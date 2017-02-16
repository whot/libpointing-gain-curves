#!/bin/bash

LD_LIBRARY_PATH=/home/whot/code/libpointing/pointing 
export LD_LIBRARY_PATH

gnuplot_file="plot-data.gnuplot"

pids=""

# $1 ... mouse cpi
# $2 ... setting
# $3 ... accel function
# $4 ... output file
run() {
    cpi="$1"
    setting="$2"
    func="$3"
    outfile="$4"

    ./curves "any:?cpi=$1" "dummy:" "$3:?setting=$setting" > $outfile &

    pids="$pids $!"
}

funcs="osx"
cpis="400"
settings="0 0.125 0.3125 0.5 0.6875 0.875 1 1.5 2 2.5 3"

filenames=""

for func in $funcs; do
    for cpi in $cpis; do
        for setting in $settings; do
            outfile="data-$func-cpi$cpi-setting$setting.dat"
            run "$cpi" "$setting" "$func" "$outfile"
            filenames="$filenames $outfile"
        done
    done
done

cat > $gnuplot_file << EOL
#!/usr/bin/gnuplot
set style data lines
set xrange[0:1000]
set xlabel "mm/s input"
set ylabel "gain"
set parametric
filenames = "$filenames"
plot for [file in filenames] file using 1:2 title file
pause -1
EOL

echo "Type anything to quit"
a=""
read a

for pid in $pids; do
    kill -9 $pid
done

for f in $filenames; do
    cat $f | sed "/.* 0\.000000/d" | sort -g | uniq -f 1 > $f.sorted
    mv $f.sorted $f
done
