#!/bin/sh
cd "`dirname \"$0\"`"
chmod 755 ./serproxy
./serproxy serproxy.cfg
