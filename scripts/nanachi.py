# メイドインアビスをみて

from flask import Flask, render_template, make_response, request, redirect, url_for
from db import get_connection
import const
import logging
import register_trace


app = Flask(__name__, static_url_path="/static")
connection = get_connection()


@app.route("/traces/register", methods=["POST"])
def trace_register():
    name = request.form["name"]
    energy = int(request.form["energy"])
    nbt_blob = request.form["nbt-blob"]
    trace_id = register_trace.register(name, nbt_blob, energy)
    return redirect(url_for("trace_summary", trace_id=trace_id))


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
    return render_template('model_summary.html', name=name)


@app.route("/model_list")
def model_list():
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT name FROM tblmodel")
    rows = cursor.fetchall()
    cursor.close()
    return render_template("model_list.html", models=rows)


@app.route("/")
def hello():
    return render_template('index.html')


if __name__ == "__main__":
    message_level = logging.WARN if const.env == "prod" else logging.INFO
    logging.basicConfig(level=message_level)
    app.run(host="localhost", port=8081)