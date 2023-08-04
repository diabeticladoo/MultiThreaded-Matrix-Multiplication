#!/bin/bash
gcc P1.c -o P1
gcc P2.c -o P2
gcc S.c -o S
./S 2 3 4
rm S
rm P1
rm P2