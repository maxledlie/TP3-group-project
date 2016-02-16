#!/bin/bash

printf "1   Graph system 1\n2   Graph system 2\n"

read SYSTEM

g++ -o sys$SYSTEM sys$SYSTEM.c -lm
./sys$SYSTEM

printf "Choose graph view:\n1   Static view from top\n2   Dynamic view\n3   Electric Field\n "

read VIEW

case $VIEW in

    1) #static topdown view

gnuplot -persist <<EOF
 set datafile separator " "
 unset key
set view map
 xequiv = 1
 yequiv=1
 set size ratio -1
 set title "System $SYSTEM"
 splot "sys$SYSTEM.dat" using 1:2:3 with points palette pointsize 3 pointtype 7

EOF


;;

    2) #dynamic graph

gnuplot -persist <<EOF
 set datafile separator " "
 unset key
 set title "System $SYSTEM"
 splot "sys$SYSTEM.dat" using 1:2:3 with points palette pointsize 3 pointtype 7
 pause mouse close

EOF
;;

    3)#Electric field graph
printf "Enter length: "
read LENGTH

printf "Enter height: " 
read HEIGHT

printf " Enter increment (if half length is even number must be odd and vice versa: "
read INCREMENT

CNTR=$LENGTH/2

gnuplot -persist <<EOF
 set datafile separator " "
set title "System $SYSTEM"
set view map 
set contour base 
set cntrparam levels increment -$CNTR, $INCREMENT, $CNTR
unset surface
set dgrid3d $LENGTH, $HEIGHT, 4 
set table 'equipo$SYSTEM.txt
splot "sys$SYSTEM.dat" with l
unset table
plot "sysvector$SYSTEM.dat" with vectors,\
'equipo$SYSTEM.txt' with l
 pause mouse close

EOF
;;

   *) #if other input, use dynamic graph (case 2)
gnuplot -persist <<EOF
 set datafile separator " "
 unset key
 set title "System $SYSTEM"
 xequiv = 1
 yequiv=1
 splot "sys$SYSTEM.dat" using 1:2:3 with points palette pointsize 3 pointtype 7
 pause mouse close

EOF

;;
esac
