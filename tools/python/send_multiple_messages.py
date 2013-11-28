#!/usr/bin/env python

import request_sender
import time
import sys

if len(sys.argv) < 4:
	print "Command syntax: python send_multiple_messages.py {number of messages} {time to send them (seconds)} {message_type} [address] [port]"
	sys.exit()

# Command line arguments
nbr_messages = int(sys.argv[1]) + 1
sending_time = float(sys.argv[2])
message_type = sys.argv[3]

if nbr_messages == 0:
	print "number of messages to send must be strictly positive"
	sys.exit()

if message_type.find(" ") != -1:
	print "message_type can't contains any spaces: '" + message_type + "'"
	sys.exit()

address = "127.0.0.1"
if len( sys.argv ) > 3:
	address = sys.argv[4]

port = 8080
if len( sys.argv ) > 4:
	port = (int)(sys.argv[5])

# Find amount of time to sleep between two send
sleep_time = sending_time / nbr_messages

# Prepare the messages sender
sender = request_sender.RequestSender()

# Send messages
for i in xrange(1, nbr_messages):
	message = message_type + " " + str(i) + " " + str((int)(time.time()))
	output  = "Sending to " + address + ":" + str(port) + " '" + message + "' ... "

	try:
		sender.prepare(address, port, message )
		sender.run()
		output += "success"
	except Exception, e:
		output += str(e)

	print output
	time.sleep( sleep_time )
