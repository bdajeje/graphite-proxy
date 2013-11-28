#!/usr/bin/env python

import subprocess
import time
import os

class Server:

    instance = None

    def start(self, port, enable_maths, enable_stats):
        self.proc = subprocess.Popen(["../../graphite-proxy", "-c", "./"])
        time.sleep(1)

    def stop(self):
        self.proc.kill()

# Wrapper functions for simplicity
def start(port, enable_maths, enable_stats):
    if Server.instance == None:
        Server.instance = Server()
        Server.instance.start(port, enable_maths, enable_stats)

def stop():
    if Server.instance != None:
        Server.instance.stop()
        Server.instance = None

import atexit
atexit.register(stop)
