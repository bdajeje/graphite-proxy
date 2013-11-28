#!/usr/bin/env python

import socket
import threading

class RequestReceiver(threading.Thread):

  def connect(self, host, port):
    try:
      self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
      self.socket.bind((host, port))
      self.socket.listen(1)
      self.start()
      return True
    except Exception, e:
      print "Error when attempting to connect: " + str(e)
      return False

  def run(self):
    self.received_messages = ""
    try:
      conn, addr = self.socket.accept()
      while 1:
        data = conn.recv(1024)
        if not data: break
        self.received_messages += data
      conn.close()
    except Exception, e:
      print "Error in receiver: " + str(e)

  def stop(self):
    try:
      if self.socket != None:
        self.socket.shutdown(socket.SHUT_RDWR)
        self.socket.close()
    except Exception, e:
      print "Error while stoping: " + str(e)

  def getReceivedMEssage(self):
    return self.received_messages

