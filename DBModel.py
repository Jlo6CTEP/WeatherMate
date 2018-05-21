import sqlite3 as sql

connection_string = "data.db"

class DBModel:

    __connection = None

    @staticmethod
    def get_connection():
        if DBModel.__connection is None:
            DBModel.__open_connection()

        return DBModel.__connection

    @staticmethod
    def close_connection():
        DBModel.__connection.close()
        DBModel.__connection = None

    @staticmethod
    def __open_connection():
        DBModel.__connection = sql.connect(connection_string)