import request_sender
import server

import time
import unittest
import os

class TestServer( unittest.TestCase ):

	def test_maths( self ):
		log_file_name = "integration_logs"

		# Remove possible previous file
		os.remove(log_file_name)

		# Start Graphite Proxy
		server.start( 9999, "true", "false" )

		# Create python request senders to send requests to the graphite proxy
		proxy_host = "127.0.0.1"
		proxy_port = 8090
		senders    = []
		for i in range(6):
			senders.append( request_sender.RequestSender( proxy_host, proxy_port, False ) )

		# Send requests in parallel to test the server threading capabilities (each RequestSender is a different thread)

		# Math category 1
		senders[0].run("test.integration.maths.1 2 0")
		senders[1].run("test.integration.maths.1 3 0")
		senders[2].run("test.integration.maths.1 5 0")

		# Math category 2
		senders[3].run("test.integration.maths.2 2 0")
		senders[4].run("test.integration.maths.2 3 0")
		senders[5].run("test.integration.maths.2 5 0")

		# Wait for the computations
		time.sleep(5)

		# Stop the server thread to be able to quit the programm properly
		server.stop()

		# Get received messages from logs
		logs_file = open(log_file_name, 'r')
		logs = logs_file.read()

		# Check received math message for rule 1 (by number of received messages)
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 10.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 2.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 5.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 3.3"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 3.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 1.5"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 1.2"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.1 100.0"), -1 )

		# Check received math message for rule 2 (by number of spend time)
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 10.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 2.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 5.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 3.3"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 3.0"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 1.5"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 1.2"), -1 )
		self.assertNotEqual( logs.find("Message added: test.integration.maths.2 100.0"), -1 )

		# Remove created logs file
		os.remove(log_file_name)
