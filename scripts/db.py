import mysql.connector
import const


def get_config():
    if const.env == 'prod':
        return {
            "host": "localhost",
            "user": "root",
            "password": "manarimo",
            "port": 3306,
            "database": "manarimodb",
            "pool_size": 32,
            "pool_name": "nanachi"
        }
    elif const.env == 'dev':
        return {
            "host": "localhost",
            "user": "root",
            "password": "",
            "port": 3306,
            "database": "manarimodb"
        }
    else:
        raise ValueError("unknown env {}".format(const.env))


def get_connection():
    return mysql.connector.connect(**get_config())


if __name__ == '__main__':
    conn = get_connection()
    cursor = conn.cursor(dictionary=True)
    cursor.execute("show tables")
    for row in cursor.fetchall():
        print(row)
    cursor.close()
    conn.close()
