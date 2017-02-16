#!/usr/bin/gnuplot
set style data lines
set xrange[0:1000]
set xlabel "mm/s input"
set ylabel "gain"
set parametric
filenames = " data-osx-cpi400-setting0.dat data-osx-cpi400-setting0.125.dat data-osx-cpi400-setting0.3125.dat data-osx-cpi400-setting0.5.dat data-osx-cpi400-setting0.6875.dat data-osx-cpi400-setting0.875.dat data-osx-cpi400-setting1.dat data-osx-cpi400-setting1.5.dat data-osx-cpi400-setting2.dat data-osx-cpi400-setting2.5.dat data-osx-cpi400-setting3.dat"
plot for [file in filenames] file using 1:2 title file
pause -1
