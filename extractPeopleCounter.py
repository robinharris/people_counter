#------------------------------------------------
#
# extractPeopleCounter.py
#
#--- Author: Robin Harris
#--- Date: 24th June 2019
#--- Version 1.0
#--- Python Version: 3.6
#
# This program is intended to be run daily by CRON.
# It should run BEFORE emailPeopleCoumter.py
# It extracts from pCounter table of robindb the number
# of counts in each 30 minute time bucket for the previous 
# 24 hours.
# The results are placed in one file for each device.
# pc1.csv and pc2.csv
#
#
#------------------------------------------------

import mysql.connector
import logging
from datetime import datetime, timedelta
import csv

dbHost = 'localhost'
dbUser = 'robinusr'
dbPassword = 'spanTHEr1v3r'
database = 'robindb'
mydb = None # global variable

# while only pc2 is live line below commented out and line 34 added
# devices = ('pc1', 'pc2')
devices = ('pc2',)

# log settings
logFile = 'extractPeopleCounter.log'
logging.basicConfig(filename=logFile,format='%(asctime)s %(message)s', level=logging.DEBUG)

logging.info("-" * 40) # visual separator
currentDate = datetime.today()
reportDate = currentDate - timedelta(days = 1)
selectStart = reportDate.strftime('%Y-%m-%d')
selectStart += ' 00:00:00'
selectEnd = currentDate.strftime('%Y-%m-%d')
selectEnd += ' 00:00:00'

try:
    mydb = mysql.connector.connect(
        host = dbHost,
        user = dbUser,
        passwd = dbPassword,
        database = database
    )
    logging.info('Opened a database connection')
except Exception as e:
    logging.exception("Unable to open a database connection")

sql = "SELECT dateTime, count FROM pCounter WHERE device = %s AND dateTime > %s AND dateTime < %s ;"
try:
    for device in devices:
        vals = (device, selectStart, selectEnd)
        mycursor=mydb.cursor()
        mycursor.execute(sql, vals)
        records = mycursor.fetchall()
        numberOfRows = len(records)
        with open(device+'.csv', "a", newline='') as csvfile:
            messageWriter = csv.writer(csvfile, delimiter = ',', quotechar = '|', quoting = csv.QUOTE_MINIMAL)
            for record in records:
                messageWriter.writerow([device, record[0], record[1]])
        logging.info("FROM: %s   TO: %s    Device: %s    Number of rows: %d",selectStart, selectEnd, device, numberOfRows)
except Exception as e:
    logging.exception('Unable to retrieve records')
