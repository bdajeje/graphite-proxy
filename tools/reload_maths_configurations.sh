#! /bin/bash

# Retrieve the pid of the running graphite_proxy_server service
PID=`initctl status graphite_proxy_server | grep /running | awk '{print $NF}'`

if [ $PID > 1 ] ; then
    # Send SUGUSR1 signal to this service
		sudo kill -SIGUSR1 $PID
		echo "Sending SIGUSR1 to graphite_proxy_server (pid: "$PID")"
else
	echo "Can't find graphite_proxy_server service"
fi