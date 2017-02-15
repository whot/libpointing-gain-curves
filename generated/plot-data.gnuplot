#!/usr/bin/gnuplot
set style data lines
set xrange[0:1000]
set xlabel "mm/s input"
set ylabel "gain"
set parametric
filenames = " data-osx-cpi400-ppi96.dat data-osx-cpi1000-ppi96.dat data-osx-cpi2000-ppi96.dat data-windows-cpi400-ppi96.dat data-windows-cpi1000-ppi96.dat data-windows-cpi2000-ppi96.dat"
plot for [file in filenames] file using 1:2 title file
pause -1
