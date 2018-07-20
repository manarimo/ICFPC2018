# oneoff to insert problems to db

import const
import db


def main():

    connection = db.get_connection()
    cursor = connection.cursor(dictionary=True)

    model_dir = const.root / "assets" / "problemsL"
    for i in range(1, 187):
        name = "LA{:03}".format(i)
        model_file = model_dir / "{}_tgt.mdl".format(name)
        print(model_file)
        with open(model_file, 'rb') as f:
            data = f.read()
        cursor.execute("INSERT INTO tblmodel (name, body) VALUES(%s, %s)", (name, data))

        cursor.execute("SELECT body FROM tblmodel WHERE name=%s", (name,))
        row = cursor.fetchone()
        body = row[b"body"]
        if body != data:
            raise ValueError("validation failed")

    connection.close()


if __name__ == '__main__':
    main()