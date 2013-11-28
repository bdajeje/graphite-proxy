import socket
import threading

class RequestSender(threading.Thread):

	def prepare(self, host, port, message):
		self.host   = host
		self.port   = port
		self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
		self.data   = message

	def run(self):
		self.socket.connect( (self.host, self.port) )
		self.socket.sendall( self.data )
		self.socket.close()
