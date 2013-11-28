import request_sender
import request_receiver
import server

import time
import unittest
import sys

class TestServer( unittest.TestCase ):

	def test_statistics( self ):
		# Create a python request receiver to receive Normal requests from the graphite proxy (emulate the Graphite Server)
		client_port = 9999
		receiver 	  = request_receiver.RequestReceiver()
		if not receiver.connect("127.0.0.1", client_port):
			print "Impossible to connect, abort."
			receiver.stop()
			sys.exit()

		# Start Graphite Proxy
		server.start( client_port, "false", "true" )

		time.sleep(4)

		server.stop()
		receiver.stop() # Stop the server thread to be able to quit the programm properly

		# Receive normal messages
		messages = receiver.getReceivedMEssage()

		self.assertNotEqual( messages.find("stats.global_buffer.messages.max"), -1 )
		self.assertNotEqual( messages.find("stats.math_buffer.messages.max"), -1 )
		self.assertNotEqual( messages.find("stats.messages.created.nbr"), -1 )
		self.assertNotEqual( messages.find("stats.statistics.messages.created.nbr"), -1 )
