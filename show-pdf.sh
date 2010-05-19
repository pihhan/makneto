#!/bin/sh

git push

ssh server-i.cipis.net << EOF
cd git/diplomka
git pull
git reset --hard
make xmensi06.pdf < /dev/null
exit
EOF

rsync server-i.cipis.net:git/diplomka/xmensi06.pdf /tmp/xmensi06.pdf
evince /tmp/xmensi06.pdf

