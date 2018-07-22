from argparse import ArgumentParser
from pathlib import Path
from db import get_connection
from hashlib import sha1


def register(name: str, blob: bytes, energy: int, author: str, comment: str):
    hasher = sha1()
    hasher.update(blob)
    digest = hasher.digest()

    connection = get_connection()
    cursor = connection.cursor(dictionary=True)
    cursor.execute("SELECT id FROM tblproblem WHERE name=%s", (name,))
    problem_id = cursor.fetchone()["id"]
    negenergy = -energy if energy is not None else None
    cursor.execute("INSERT INTO tbltrace (problem_id, body, score, sha1) VALUES (%s, %s, %s, %s) ON DUPLICATE KEY UPDATE score=%s", (problem_id, blob, negenergy, digest, negenergy))
    trace_id = cursor.lastrowid
    cursor.execute("REPLACE INTO tbltrace_metadata (trace_id, energy, author, comment) VALUES (%s, %s, %s, %s)", (trace_id, energy, author, comment))
    cursor.close()
    connection.commit()
    connection.close()
    return trace_id


def main():
    parser = ArgumentParser()
    parser.add_argument("model_name", help="model name. e.g. LA001")
    parser.add_argument("nobita", type=Path, help="nbt file")
    parser.add_argument("author", help="author of the nbt")
    parser.add_argument("--energy", type=int, help="energy. use official checker to calculate!", default=None)
    parser.add_argument("--comment", help="comment (if any)", default="")
    args = parser.parse_args()
    with args.nobita.open("rb") as f:
        blob = f.read()
    trace_id = register(args.model_name, blob, args.energy, args.author, args.comment)
    print("registered trace_id: {}".format(trace_id))


if __name__ == '__main__':
    main()