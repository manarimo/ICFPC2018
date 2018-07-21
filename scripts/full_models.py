# oneoff to insert problems to db

import const
import db

files = """
FR035_src.mdl
FR039_src.mdl
FR047_src.mdl
FR066_src.mdl
FR070_tgt.mdl
FR074_src.mdl
FR076_src.mdl
FR077_src.mdl
FR086_src.mdl
FR089_src.mdl
FR101_src.mdl
FR103_src.mdl
FR113_src.mdl
FR114_tgt.mdl
FR115_src.mdl
"""

def main():

    connection = db.get_connection()
    cursor = connection.cursor(dictionary=True)

    model_dir = const.root / "assets" / "problemsF"
    for file in files.split():
        name = file[0:-4]
        model_file = model_dir / file
        print(model_file)
        with model_file.open('rb') as f:
            data = f.read()
        cursor.execute("INSERT INTO tblmodel (name, body) VALUES(%s, %s)", (name, data))
        connection.commit()
        cursor.execute("SELECT body FROM tblmodel WHERE name=%s", (name,))
        row = cursor.fetchone()
        body = row[b"body"]
        if body != data:
            raise ValueError("validation failed")

    connection.close()


if __name__ == '__main__':
    main()