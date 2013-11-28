import request_sender
import request_receiver
import server

import time
import unittest
import sys

def run_test(tester, proxy_port, is_udp):
	# Create a python request receiver to receive Normal requests from the graphite proxy (emulate the Graphite Server)
	client_port = 9999
	receiver 	  = request_receiver.RequestReceiver()
	if not receiver.connect("127.0.0.1", client_port):
		print "Impossible to connect, abort."
		receiver.stop()
		sys.exit()

	# Start Graphite Proxy
	server.start( client_port, "false", "false" )

	# Create python request senders to send requests to the graphite proxy
	proxy_host = "127.0.0.1"
	senders    = []
	for i in range(4):
		senders.append( request_sender.RequestSender( proxy_host, proxy_port, is_udp ) )

	# Send requests in parallel to test the server threading capabilities (each RequestSender is a different thread)
	senders[0].run("Integration.test.1 0 0")
	senders[1].run("Integration.test.1 50 0")
	senders[2].run("Integration.test.2 100 0")
	senders[3].run("Integration.test.3 9999 0")

	time.sleep(2)

	server.stop()
	receiver.stop() # Stop the server thread to be able to quit the programm properly

	# Receive normal messages
	tester.assertNotEqual( receiver.getReceivedMEssage().find("Integration.test.1 0.000000 0\nIntegration.test.1 50.000000 0\nIntegration.test.2 100.000000 0\nIntegration.test.3 9999.000000 0\n"), -1 )

class TestServer( unittest.TestCase ):

	def test_pass_through_tcp( self ):
		run_test(self, 8090, False)

	def test_pass_through_udp( self ):
		run_test(self, 8099, True)
