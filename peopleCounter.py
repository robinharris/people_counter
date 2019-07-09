import mysql.connector
import paho.mqtt.client as mqtt
import logging
import datetime

# settings
logging.basicConfig(filename="peopleCounter.log", format='%(asctime)s %(message)s', level=logging.DEBUG)
mqttClient = mqtt.Client("rjh", clean_session=True)
broker = "172.26.6.1"
mqttUser = "connectedhumber"
mqttPassword = "3fds8gssf6"

def on_message(client, userdata, message):
  global mydb
  rcvdMessage = str(message.payload.decode("utf-8"))
  logging.info("Received message: %s", rcvdMessage)
  dateTime, count, deviceID = rcvdMessage.split(',')
  try:
    mycursor = mydb.cursor()
    sql = "INSERT INTO pCounter (dateTime, count, device) VALUES (%s, %s, %s)"
    vals =(dateTime, count, deviceID)
    mycursor.execute(sql, vals)
    mydb.commit()
    logging.info("Inserted %s records", mycursor.rowcount)
  except Exception as e:
    logging.info("Failed to insert record")

def on_subscribe(client, object, mid, granted_qos):
  logging.info("Subscribed")

def on_connect(client, object, flags, rc):
    logging.info("Connected to mqtt broker")
    mqttClient.subscribe("pir")

def on_disconnect(client, userdata, rc):
  logging.info("Disconnected: %s", str(rc))

# set callbacks
mqttClient.on_message=on_message
mqttClient.on_subscribe=on_subscribe
mqttClient.on_connect=on_connect
mqttClient.on_disconnect=on_disconnect

mqttClient.username_pw_set(username=mqttUser, password=mqttPassword)
mqttClient.connect(broker, keepalive=60)

#connect to database
try:
  mydb = mysql.connector.connect(
    host="localhost",
    user="robinusr",
    passwd="spanTHEr1v3r",
    database="robindb"
  )
  logging.info("Connected to database")
except:
    logging.info("Failed to connect to database")

mqttClient.loop_forever()
