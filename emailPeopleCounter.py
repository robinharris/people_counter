#------------------------------------------------
#
# emailPeopleCounter.py
#
#--- Author: Robin Harris
#--- Date: 24th June 2019
#--- Version 1.0
#--- Python Version: 3.6
#
# This program is intended to be run daily by CRON
# following extractPeopleCouinter.py
# It creates an email to be sent via Google with 2
# attachments - pc1.csv and pc2.csv
#
#------------------------------------------------

import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders

emailUser = 'robin.harris@ayelandsassociates.co.uk'
emailRecipient = 'rh@connectedhumber.org'
emailPassword = 'spanTHEr1v3r'
subject = 'People Counter Files'
body = 'Here are the files for yesterday'

message = MIMEMultipart()
message['From'] = emailUser
message['To'] = emailRecipient
message['subject'] = subject
message.attach(MIMEText(body,'plain'))

# line below is for 2 devices / files.
# files = ('pc1.csv', 'pc2.csv')
files = ('pc2.csv',)
for filename in files:
    attachment = open(filename, 'rb')
    part = MIMEBase('application', 'octet-stream')
    part.set_payload(attachment.read())
    encoders.encode_base64(part)
    part.add_header('Content-Disposition', 'attachment: filename= ' + filename)
    message.attach(part)
textMessage = message.as_string()
server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
server.login(emailUser, emailPassword)
server.sendmail(emailUser, emailRecipient, textMessage)
server.quit()