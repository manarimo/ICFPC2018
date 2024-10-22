# メイドインアビスをみて
import json
from pathlib import Path

from flask import Flask, render_template, make_response, send_from_directory, request, redirect, url_for, Response
from flask_cors import CORS
from db import get_connection
import const
import logging
import register_trace
from tempfile import NamedTemporaryFile
import submit
import api


app = Flask(__name__, static_url_path="/static")
app.config['MAX_CONTENT_LENGTH'] = 50 * 1024 * 1024
CORS(app)

@app.route("/favicon.ico")
def favicon():
    return app.send_static_file("favicon.ico")


@app.route("/assets/<path:path>")
def assets(path):
    return send_from_directory(const.root / 'official-tools' / 'assets', path)


@app.route("/api/pending_traces")
def pending_traces():
    connection = get_connection()
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT p.name AS problem_name, trace_id, energy, author, comment, "
                   "src.name AS src_model, tgt.name AS tgt_model, p.type AS problem_type, tbltrace_metadata.s3url "
                   "FROM tbltrace_metadata JOIN tbltrace on trace_id = tbltrace.id "
                   "JOIN tblproblem p ON p.id = tbltrace.problem_id "
                   "LEFT JOIN tblmodel src ON p.src_model_id = src.id "
                   "LEFT JOIN tblmodel tgt ON p.tgt_model_id = tgt.id "
                   "WHERE tbltrace_metadata.energy_autoscorer IS NULL AND tbltrace_metadata.failed IS NULL")
    traces = cursor.fetchall()
    cursor.close()
    connection.commit()
    return Response(json.dumps({"traces": traces}), mimetype="application/json")


@app.route("/best_traces", methods=["GET", "POST"])
def best_traces():
    connection = get_connection()
    traces = {}
    references = {}
    cursor = connection.cursor(dictionary=True)
    lightning = request.args.get('lightning') == 'true'
    autoscorer = True
    cursor.execute("SELECT p.name AS problem_name, trace_id, tbltrace_metadata.energy, energy_autoscorer, author, comment,"
                   "  r.energy AS ranking_energy, r.name AS ranking_name "
                   "FROM tbltrace_metadata JOIN tbltrace on trace_id = tbltrace.id "
                   "JOIN tblproblem p ON p.id = tbltrace.problem_id "
                   "LEFT OUTER JOIN tblofficial_ranking r ON p.id = r.problem_id "
                   "WHERE p.is_lightning = %s AND p.dummy_problem = FALSE",
                   (lightning,))
    key = "energy_autoscorer" if autoscorer else "energy"
    def energy_order(energy):
        return energy is None, energy
    for trace in cursor.fetchall():
        trace['energy_to_use'] = trace['energy_autoscorer']

        problem_name = trace["problem_name"]
        if problem_name not in traces:
            traces[problem_name] = trace
        elif energy_order(trace[key]) < energy_order(traces[problem_name][key]):
            traces[problem_name] = trace
        if trace["author"] == 'icfpc2018':
            references[problem_name] = trace["energy_autoscorer"]
    cursor.close()
    connection.commit()
    for k in traces.keys():
        traces[k]['ref_energy'] = references.get(k)

    if request.method == "POST":
        cursor = connection.cursor(dictionary=True)
        blobs = {}  # todo: do this in single query
        for trace in traces.values():
            cursor.execute("SELECT body FROM tbltrace WHERE id=%s", (trace["trace_id"], ))
            blob = cursor.fetchone()["body"]
            blobs[trace["problem_name"]] = blob
        cursor.close()
        connection.commit()
        with NamedTemporaryFile() as zf:
            zipfile_path = Path(zf.name)
            submit.generate_zip(zipfile_path, blobs)
            digest, public_url = submit.upload_to_s3(Path(zf.name))
            api.do_submit(public_url, digest)
            return render_template("submit_result.html", zipfile_url=public_url)
    else:
        return render_template("best_traces.html", traces=sorted(traces.values(), key=lambda trace: trace["problem_name"]), autoscorer=autoscorer)


@app.route("/traces/register", methods=["POST"])
def trace_register():
    try:
        name = request.form["name"]
        author = request.form["author"]
        comment = request.form["comment"]
        if "energy" in request.form:
            energy = int(request.form["energy"])
        else:
            energy = None
        if "nbt-blob" in request.files:
            nbt_blob = request.files["nbt-blob"].read()
        else:
            nbt_blob = None
        s3_url = request.form.get('s3url', None)
        sha1sum = request.form.get('sha1sum', None)
        trace_id = register_trace.register(name, nbt_blob, energy, author, comment, s3_url, sha1sum)
        return Response(json.dumps({"status": "success", "trace_id": trace_id}), content_type='application/json')
    except Exception as e:
        return Response(json.dumps({"status": "failure", "message": str(e)}), content_type='application/json')


@app.route("/traces/<int:trace_id>/blob")
def trace_blob(trace_id: int):
    connection = get_connection()
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT body FROM tbltrace WHERE id=%s", (trace_id,))
    blob = cursor.fetchone()["body"]
    cursor.close()
    connection.commit()
    response = make_response(blob)
    response.headers.set('Content-Type', 'application/octet-stream')
    response.headers.set('Content-Disposition', 'attachment', filename='trace{}.nbt'.format(trace_id))
    return response


@app.route("/traces/<int:trace_id>")
def trace_summary(trace_id: int):
    connection = get_connection()
    cursor = connection.cursor(dictionary=True)
    cursor.execute(
        "SELECT tbltrace.id AS id, tblproblem.name AS `name`, tm.author AS author, tm.comment AS comment, tm.energy AS energy,"
        "tm.submit_time AS submit_time, tm.energy_autoscorer AS energy_autoscorer, tm.s3url AS s3url "
        "FROM tbltrace JOIN tblproblem ON tbltrace.problem_id = tblproblem.id "
        "JOIN tbltrace_metadata tm ON tbltrace.id = tm.trace_id "
        "WHERE tbltrace.id=%s",
        (trace_id,))
    row = cursor.fetchone()
    print(row)
    row["submit_time_string"] = row["submit_time"].strftime('%Y-%m-%d %H:%M:%S')
    cursor.close()
    connection.commit()
    return render_template("trace_summary.html", trace=row)

@app.route("/traces/<int:trace_id>/update-autoscorer", methods=["POST"])
def update_autoscorer(trace_id: int):
    connection = get_connection()
    try:
        if 'energy' in request.form:
            energy = int(request.form["energy"])
            failed = False
        else:
            energy = None
            failed = True
        cursor = connection.cursor(dictionary=True)
        cursor.execute(
            "UPDATE tbltrace_metadata SET energy_autoscorer = %s, failed = %s WHERE trace_id = %s",
            (energy, failed, trace_id,)
        )
        connection.commit()
        return Response(json.dumps({"status": "success"}), content_type='application/json')
    except Exception as e:
        return Response(json.dumps({"status": "failure", "message": str(e)}), content_type='application/json')


@app.route("/models/<name>/blob")
def model_blob(name: str):
    connection = get_connection()
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT body FROM tblmodel WHERE name=%s", (name,))
    blob = cursor.fetchone()["body"]
    cursor.close()
    connection.commit()
    response = make_response(blob)
    response.headers.set('Content-Type', 'application/octet-stream')
    response.headers.set('Content-Disposition', 'attachment', filename='%s_tgt.mdl' % name)
    return response


@app.route("/models/<name>")
def model_summary(name: str):
    connection = get_connection()
    tracecursor = connection.cursor(dictionary=True)
    tracecursor.execute(
        "SELECT tm.trace_id, tm.energy, tm.author, tm.comment, tm.submit_time, tm.energy_autoscorer "
        "FROM tbltrace JOIN tbltrace_metadata tm ON tbltrace.id = tm.trace_id "
        "JOIN tblmodel ON tbltrace.model_id = tblmodel.id WHERE tblmodel.name=%s ORDER BY tm.energy IS NULL, tm.energy ASC",
        (name,))
    tracerows = tracecursor.fetchall()
    tracerows = [dict(row, **{ "submit_time_string": row["submit_time"].strftime('%Y-%m-%d %H:%M:%S') }) for row in tracerows]
    tracecursor.close()
    connection.commit()

    modelcursor = connection.cursor(dictionary=True)
    modelcursor.execute(
        "SELECT m.name AS name, meta.r AS r, meta.fill_count AS fill_count, "
          "meta.num_components AS num_components, "
          "meta.largest_component_size AS largest_component_size, "
          "meta.max_depth AS max_depth, meta.num_void_spaces AS num_void_spaces "
          "FROM tblmodel m "
          "JOIN tblmodel_metadata meta ON m.id = meta.model_id "
          "WHERE m.name=%s",
          (name,))
    model = modelcursor.fetchone()
    modelcursor.close()
    connection.commit()

    return render_template('model_summary.html', name=name, traces=tracerows, model=model)


@app.route("/model_list")
def model_list():
    connection = get_connection()
    get_list_sql = "SELECT m.name AS name, meta.r AS r, meta.fill_count AS fill_count, "\
          "meta.num_components AS num_components, "\
          "meta.largest_component_size AS largest_component_size, "\
          "meta.max_depth AS max_depth, meta.num_void_spaces AS num_void_spaces "\
          "FROM tblmodel m "\
          "JOIN tblmodel_metadata meta ON m.id = meta.model_id"
    cursor = connection.cursor(dictionary=True)
    cursor.execute(get_list_sql)
    rows = cursor.fetchall()
    cursor.close()
    connection.commit()

    return render_template("model_list.html", models=rows)

@app.route("/problem_list")
def problem_list():
    connection = get_connection()
    lightning = request.args.get('lightning') == 'true'
    cursor = connection.cursor(dictionary=True)
    cursor.execute(
        "SELECT p.id, p.name, p.type, src.name AS src_name, tgt.name AS tgt_name FROM tblproblem p "
        "LEFT JOIN tblmodel src ON p.src_model_id = src.id "
        "LEFT JOIN tblmodel tgt ON p.tgt_model_id = tgt.id "
        "WHERE p.is_lightning=%s",
        (lightning,))
    rows = cursor.fetchall()
    cursor.close()
    connection.commit()

    return render_template("problem_list.html", problems=rows)

@app.route("/problems/<name>")
def problem_summary(name: str):
    connection = get_connection()
    tracecursor = connection.cursor(dictionary=True)
    tracecursor.execute(
        "SELECT tm.trace_id, tm.energy, tm.author, tm.comment, tm.submit_time, tm.energy_autoscorer, tm.s3url "
        "FROM tbltrace JOIN tbltrace_metadata tm ON tbltrace.id = tm.trace_id "
        "JOIN tblproblem ON tbltrace.problem_id = tblproblem.id WHERE tblproblem.name=%s ORDER BY tm.energy IS NULL, tm.energy ASC",
        (name,))
    tracerows = tracecursor.fetchall()
    tracerows = [dict(row, **{ "submit_time_string": row["submit_time"].strftime('%Y-%m-%d %H:%M:%S') }) for row in tracerows]
    tracecursor.close()
    connection.commit()

    problemcursor = connection.cursor(dictionary=True)
    problemcursor.execute(
        "SELECT p.id, p.name, p.type, src.name AS src_name, tgt.name AS tgt_name FROM tblproblem p "
        "LEFT JOIN tblmodel src ON p.src_model_id = src.id "
        "LEFT JOIN tblmodel tgt ON p.tgt_model_id = tgt.id "
        "WHERE p.name=%s",
        (name,))
    problem = problemcursor.fetchone()
    problemcursor.close()
    connection.commit()

    return render_template('problem_summary.html', name=name, traces=tracerows, problem=problem)

@app.route("/rankings/update", methods=['POST'])
def update_ranking():
    connection = get_connection()
    payload = request.json
    problem_names = [e['problem_name'] for e in payload]
    cursor = connection.cursor(dictionary=True)
    placeholder = ','.join(['%s'] * len(problem_names))
    cursor.execute("SELECT id, name FROM tblproblem WHERE name IN (%s)" % placeholder, problem_names)

    problem_map = dict()
    for row in cursor.fetchall():
        problem_map[row['name']] = row['id']
    cursor.close()

    values = []
    for entry in payload:
        values.append((problem_map[entry['problem_name']], entry['name'], int(entry['score'])))

    cursor = connection.cursor(dictionary=True)
    cursor.executemany(
        "INSERT INTO tblofficial_ranking (problem_id, name, energy) VALUES (%s, %s, %s) "
        "ON DUPLICATE KEY UPDATE "
        "name=IF(energy > VALUES(energy), VALUES(name), name), "
        "energy=LEAST(energy, VALUES(energy))",
    values)
    cursor.close()
    connection.commit()

    return ('', 200)

@app.route("/")
def hello():
    return render_template('index.html')


if __name__ == "__main__":
    message_level = logging.WARN if const.env == "prod" else logging.INFO
    logging.basicConfig(level=message_level)
    app.run(host="localhost", port=8081, processes=12, threaded=False)