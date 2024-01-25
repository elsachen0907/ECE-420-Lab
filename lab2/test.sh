#! /bin/bash
# Run your client for 100 times
# 
# 
# Usage:
#	./test.sh
# Notes:
#	This shell script should be in the same directory as your
#	client implementation and your client should have portnumber 
#	and the number of strings in theArray as command line paramters;
#

if [ $# -lt 3 ]; then
	echo "arraySize serverIp serverPort"
	exit -1
fi

# Parameters
Duplicates=100
# Port Number
port=${3}
# The number of strings in theArray
arraysize=${1}
# IP of the server
IP=${2}

clear

echo "Start..."
ATTEMPT=0
while [[ $ATTEMPT -ne $Duplicates ]]; do
	let ATTEMPT+=1
	echo "Attempt ${ATTEMPT} started."
	./client $arraysize $IP $port
	sleep 0.5
done
