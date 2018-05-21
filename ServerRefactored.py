import socket
from datetime import datetime as dt
import DBManager

sock = socket.socket()
sock.bind(('', 9090))
sock.listen(1)

db = DBManager.DBManager("data")


def receive():
    conn, addr = sock.accept()
    print('connected:', addr)
    global income
    income = str(conn.recv(1024))[2:-1].split(' ')
    conn.send(b'1')
    conn.close()
    income[0] = int(income[0])
    income[1] = int(income[1])

    return income


def database_add(data, time=None):
    if time is None:
        time = dt.today()

    db.add_data(data[0], data[1], time)


def printDB():
    return db.get_all_data()


def clearDB():
    db.clear_database()


# clearDB()
print(printDB())
while True:
    try:
        data = receive()
        database_add(data)

    except ConnectionResetError:
        continue