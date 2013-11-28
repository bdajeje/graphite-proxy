#!/usr/bin/env python

import request_sender
import time
import sys

if len(sys.argv) != 4:
	print "Command syntax: python send_merge_messages.py {number of messages} {message_type} {start value}"
	sys.exit()

# Command line arguments
nbr_messages  = int(sys.argv[1])
message_type  = sys.argv[2]
message_value = (int)(sys.argv[3])
message_time  = (int)(time.time())

if nbr_messages == 0:
	print "number of messages to send must be strictly positive"
	sys.exit()

if message_type.find(" ") != -1:
	print "message_type can't contains any spaces: '" + message_type + "'"
	sys.exit()

# Prepare the messages sender
sender = request_sender.RequestSender()

# Create merge message
message = ""
for i in xrange(0, nbr_messages):
	message += message_type + " " + str(message_value + i) + " " + str(message_time + i) + "\n"

# Send messages
try:
	output = "Sending:\n" + message
	sender.prepare("127.0.0.1", 8080, message)
	sender.run()
	output += "\nRECEIVED"
except Exception, e:
	output += str(e)

print output
