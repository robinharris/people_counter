import mysql.connector

mydb = mysql.connector.connect(
  host="localhost",
  user="robinusr",
  passwd="spanTHEr1v3r",
  database="robindb"
)

mycursor = mydb.cursor()

sql = "INSERT INTO peopleCounter (dateTime, count) VALUES (%s, %s)"

val = ("2018-11-16 17:05:34", 23)
mycursor.execute(sql, val)

mydb.commit()

print(mycursor.rowcount, "record inserted.")