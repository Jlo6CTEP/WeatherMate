import socket
import sqlite3
from datetime import datetime as dt

income = []

sock = socket.socket()
sock.bind(('', 9090))
sock.listen(1)


def receive():
    conn, addr = sock.accept()
    print('connected:', addr)
    global income
    income = str(conn.recv(1024))[2:-1].split(' ')
    conn.send(b'1')
    conn.close()
    income[0] = int(income[0])
    income[1] = int(income[1])


def databaseAdd():
    conn = sqlite3.connect('data.db')
    cursor = conn.cursor()
    global income
    cDate, cTime = dt.today().strftime("%Y-%m-%d %H:%M:%S").split(' ')
    cursor.execute(
        "INSERT INTO data (temperature, pressure, currentdate, currenttime) VALUES ({0},{1},'{2}','{3}')".format(
            income[0], income[1], cDate, cTime))
    conn.commit()
    conn.close()
    print("New record: {0} {1} {2} {3}".format(income[0], income[1], cDate, cTime))


def printDB():
    conn = sqlite3.connect('data.db')
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM data")
    re = cursor.fetchall()
    conn.close()
    return re


def clearDB():
    conn = sqlite3.connect('data.db')
    cursor = conn.cursor()
    cursor.execute("DELETE FROM data")
    conn.commit()
    conn.close()


# clearDB()
# print(printDB())
while True:
    try:
        receive()
        databaseAdd()
    except ConnectionResetError:
        continue
