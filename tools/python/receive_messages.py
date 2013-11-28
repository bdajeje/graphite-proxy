#!/usr/bin/env python

import request_receiver
import sys

port = 8081
if len(sys.argv) == 2:
	port = (int)(sys.argv[1])

address  = "127.0.0.1"
stop 		 = False
receiver = request_receiver.RequestReceiver(address, port)
receiver.connect()

print "Connected on %s" % (address + ':' + str(port))

# Receive incoming messages
while ( stop == False ):
	result = receiver.listen();
	print result
