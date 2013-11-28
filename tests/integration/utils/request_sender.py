#!/usr/bin/env python

import socket
import threading

class RequestSender(threading.Thread):

  def __init__(self, host, port, is_udp):
    self.host = host
    self.port = port

    self.protocol = socket.SOCK_STREAM
    if is_udp == True:
      self.protocol = socket.SOCK_DGRAM

    self.socket = socket.socket( socket.AF_INET, self.protocol )

  def run(self, message):
    self.socket.connect( (self.host, self.port) )

    if self.protocol == socket.SOCK_STREAM:
      self.socket.sendall( message )
    else:
      self.socket.sendto( message, (self.host, self.port) )

    self.socket.close()
