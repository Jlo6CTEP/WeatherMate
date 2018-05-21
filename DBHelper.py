import DBModel
import sqlite3


class DBHelper:

    __connection = None

    @staticmethod
    def execute_query(query):
        if DBHelper.__connection is None:
            DBHelper.__connection = DBModel.DBModel.get_connection()

        cursor = DBHelper.__connection.cursor()

        try:
            cursor.execute(query)

        except:
            DBHelper.reset_connection()
            cursor.execute(query)

        return cursor

    @staticmethod
    def insert_entry(table : str, values : [], immediate_commit=True):

        if not len(values):
            return

        query = "INSERT INTO " + table + " VALUES ("
        for value in values:
            query += str(value) + ', '

        query = query[:-2] + ');'

        DBHelper.execute_query(query)

        if immediate_commit:
            DBHelper.__connection.commit()

    @staticmethod
    def get_all_entries(table):
        cursor = DBHelper.execute_query("SELECT * FROM " + table)
        rows = cursor.fetchall()

        return rows

    @staticmethod
    def reset_connection():
        DBModel.DBModel.close_connection()
        DBHelper.__connection = DBModel.DBModel.get_connection()

    @staticmethod
    def clear_data_from_table(table):
        query = "DELETE FROM {table};".format(table=table)
        DBHelper.execute_query(query)
        DBHelper.__connection.commit()