#!/bin/bash
HOST='192.168.1.29'
PORT='1337'
USER=' '
PASSWD=' '
FILE='build/aes.vpk'

ftp -n $HOST $PORT << END_SCRIPT
quote USER $USER
quote PASS $PASSWD
binary
send $FILE ux0:/aes.vpk
quit
END_SCRIPT
exit 0
