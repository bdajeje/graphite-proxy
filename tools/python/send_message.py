#!/usr/bin/env python

import request_sender
import sys
import time

message = sys.argv[1]

# Add extra space at the end of the message if not here
if message[-1] != " ":
  message += " "

# Prepend current timestamp to the message
timestamp = int(time.time())
message += str( timestamp )

# Retrieve address
address = "127.0.0.1"
if len(sys.argv) > 2: address = sys.argv[2]

# Retrieve port
port = 8090
if len(sys.argv) > 3: port = (int)(sys.argv[3])

sender = request_sender.RequestSender()
sender.prepare( address, port, message )
print "Sending => %s" % message
sender.run()
