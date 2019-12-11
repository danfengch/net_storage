#!/bin/bash

mkdir -p  /etc/boa
cp ./boa.conf /etc/boa/
chmod u+x boa CfgMgr
./boa &
./CfgMgr &

