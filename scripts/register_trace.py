from argparse import ArgumentParser
from pathlib import Path
from db import get_connection


def register(name: str, nbt_path: Path, energy: int):
    with nbt_path.open("rb") as f:
        blob = f.read()
    connection = get_connection()
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT id FROM tblmodel WHERE name=%s", (name,))
    model_id = cursor.fetchone()["id"]
    cursor.execute("INSERT INTO tbltrace (model_id, body, score) VALUES (%s, %s, %s)", (model_id, blob, -energy))
    trace_id = cursor.lastrowid
    cursor.execute("INSERT INTO tbltrace_metadata (trace_id, energy) VALUES (%s, %s)", (trace_id, energy))
    cursor.close()
    connection.commit()
    connection.close()
    return trace_id


def main():
    parser = ArgumentParser()
    parser.add_argument("model_name", help="model name. e.g. LA001")
    parser.add_argument("nobita", type=Path, help="nbt file")
    parser.add_argument("energy", type=int, help="energy. use official checker to calculate!")
    args = parser.parse_args()
    trace_id = register(args.model_name, args.nobita, args.energy)
    print("registered trace_id: {}".format(trace_id))


if __name__ == '__main__':
    main()