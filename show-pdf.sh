#!/bin/sh

git push

ssh server-i.cipis.net << EOF
cd git/diplomka
git pull
make xmensi06.pdf < /dev/null
exit
EOF

rsync server-i.cipis.net:git/diplomka/xmensi06.pdf /tmp/xmensi06.pdf
evince /tmp/xmensi06.pdf

