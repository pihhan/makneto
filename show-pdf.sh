#!/bin/sh

ssh server-i.cipis.net << EOF
cd diplomka-work
git pull
make
exit
EOF

sftp server-i.cipis.net:diplomka-work/xmensi06.pdf /tmp/xmensi06.pdf
evince /tmp/xmensi06.pdf

