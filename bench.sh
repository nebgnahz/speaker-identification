#!/bin/sh

set -e

echo "" > log
for cc in 6 7 8 9 10 11 12
do
    ./speaker-recognition -c $cc -g 16 -t 10 2>> log
done

for gmm in 12 16 20 24 28 32
do
    ./speaker-recognition -c 12 -g $gmm -t 10 2>> log
done

for tt in 10 15 20 25 30
do
    ./speaker-recognition -c 12 -g 16 -t $tt 2>> log
done
