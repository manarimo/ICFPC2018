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
CORS(app)
connection = get_connection()


@app.route("/assets/<path:path>")
def assets(path):
    return send_from_directory(const.root / 'official-tools' / 'assets', path)


@app.route("/best_traces", methods=["GET", "POST"])
def best_traces():
    traces = {}
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT `name` AS model_name, trace_id, energy "
                   "FROM tbltrace_metadata JOIN tbltrace on trace_id = tbltrace.id "
                   "JOIN tblmodel ON tblmodel.id = tbltrace.model_id")
    for trace in cursor.fetchall():
        model_name = trace["model_name"]
        if model_name not in traces:
            traces[model_name] = trace
        elif trace["energy"] < traces[model_name]["energy"]:
            traces[model_name] = trace
    cursor.close()

    if request.method == "POST":
        cursor = connection.cursor(dictionary=True)
        blobs = {}  # todo: do this in single query
        for trace in traces.values():
            cursor.execute("SELECT body FROM tbltrace WHERE id=%s", (trace["trace_id"], ))
            blob = cursor.fetchone()[b"body"]
            blobs[trace["model_name"]] = blob
        cursor.close()
        with NamedTemporaryFile() as zf:
            zipfile_path = Path(zf.name)
            submit.generate_zip(zipfile_path, blobs)
            digest, public_url = submit.upload_to_s3(Path(zf.name))
            api.do_submit(public_url, digest)
            return render_template("submit_result.html", zipfile_url=public_url)
    else:
        return render_template("best_traces.html", traces=traces.values())


@app.route("/traces/register", methods=["POST"])
def trace_register():
    try:
        name = request.form["name"]
        energy = int(request.form["energy"])
        nbt_blob = request.files["nbt-blob"].read()
        trace_id = register_trace.register(name, nbt_blob, energy)
        return Response(json.dumps({"status": "success", "trace_id": trace_id}), content_type='application/json')
    except Exception as e:
        return Response(json.dumps({"status": "failure", "trace_id": str(e)}), content_type='application/json')


@app.route("/traces/<int:trace_id>/blob")
def trace_blob(trace_id: int):
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT body FROM tbltrace WHERE id=%s", (trace_id,))
    blob = cursor.fetchone()[b"body"]
    cursor.close()
    response = make_response(blob)
    response.headers.set('Content-Type', 'application/octet-stream')
    response.headers.set('Content-Disposition', 'attachment', filename='trace{}.nbt'.format(trace_id))
    return response


@app.route("/traces/<int:trace_id>")
def trace_summary(trace_id: int):
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT tbltrace.id AS id, model_id, tblmodel.name AS `name` FROM tbltrace JOIN tblmodel ON tbltrace.model_id = tblmodel.id WHERE tbltrace.id=%s", (trace_id,))
    row = cursor.fetchone()
    cursor.close()
    return render_template("trace_summary.html", trace=row)


@app.route("/models/<name>/blob")
def model_blob(name: str):
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT body FROM tblmodel WHERE name=%s", (name,))
    blob = cursor.fetchone()[b"body"]
    cursor.close()
    response = make_response(blob)
    response.headers.set('Content-Type', 'application/octet-stream')
    response.headers.set('Content-Disposition', 'attachment', filename='%s_tgt.mdl' % name)
    return response


@app.route("/models/<name>")
def model_summary(name: str):
    cursor = connection.cursor(dictionary=True)
    cursor.execute(
        "SELECT tbltrace_metadata.trace_id, tbltrace_metadata.energy "
        "FROM tbltrace JOIN tbltrace_metadata ON tbltrace.id = tbltrace_metadata.trace_id "
        "JOIN tblmodel ON tbltrace.model_id = tblmodel.id WHERE tblmodel.name=%s",
        (name,))
    rows = cursor.fetchall()
    cursor.close()
    return render_template('model_summary.html', name=name, traces=rows)


@app.route("/model_list")
def model_list():
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
    return render_template("model_list.html", models=rows)


@app.route("/")
def hello():
    return render_template('index.html')


if __name__ == "__main__":
    message_level = logging.WARN if const.env == "prod" else logging.INFO
    logging.basicConfig(level=message_level)
    app.run(host="localhost", port=8081, debug=True)