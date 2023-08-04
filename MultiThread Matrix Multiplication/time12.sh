#!/bin/bash
gcc P1.c -o P1
gcc P2.c -o P2
gcc timing12.c -o timing12
./timing12 2 3 4
rm timing12
rm P1
rm P2