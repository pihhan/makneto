#!/bin/sh

git push

ssh server-i.cipis.net << EOF
cd diplomka-work
git pull
make xmensi06.pdf < /dev/null
exit
EOF

rsync server-i.cipis.net:diplomka-work/*.pdf /tmp/xmensi06.pdf
evince /tmp/xmensi06.pdf

