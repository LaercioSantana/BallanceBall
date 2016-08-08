#!/bin/bash

./plotSample.m&
./bin/app $1 $2 -s ./samples.dat
pkill -f /usr/bin/octave-cli\ -qf\ ./plotSample.m
