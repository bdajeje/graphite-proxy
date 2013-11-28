import socket

# Class to represent the Graphite Server
class RequestReceiver:

	def __init__(self, host, port):
		self.host = host
		self.port = port
		self.data = ""

	def connect(self):
		self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
		self.socket.bind( (self.host, self.port) )
		self.socket.listen(1)

	def listen(self):
		conn, addr = self.socket.accept()
		self.data = ""
		while 1:
			received = conn.recv(4096)
			if received:
				self.data += received
			else:
				break
		conn.close()
		return self.data
