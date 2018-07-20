# メイドインアビスをみて

from flask import Flask, render_template, make_response, send_from_directory
from flask_cors import CORS
from db import get_connection
import const
import logging


app = Flask(__name__)
CORS(app)
connection = get_connection()


@app.route("/assets/<path:path>")
def assets(path):
    return send_from_directory(const.root / 'official-tools' / 'assets', path)

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