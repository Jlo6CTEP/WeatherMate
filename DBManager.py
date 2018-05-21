from DBHelper import DBHelper as db


class DBManager:
    __table = ""

    def __init__(self, table="data"):
        self.__table = table

    def add_data(self, temperature, pressure, time):
        date, time = time.strftime("%Y-%m-%d %H:%M:%S").split(' ')
        db.insert_entry(self.__table, [temperature, pressure, date, time])
        print("New record: {0} {1} {2} {3}".format(temperature, pressure, date, time))

    def get_all_data(self):
        return db.get_all_entries(self.__table)

    def clear_database(self):
        db.clear_data_from_table(self.__table)