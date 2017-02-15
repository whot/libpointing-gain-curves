#!/bin/bash

LD_LIBRARY_PATH=/home/whot/code/libpointing/pointing 
export LD_LIBRARY_PATH

gnuplot_file="plot-data.gnuplot"

pids=""

# $1 ... mouse cpi
# $2 ... display ppi
# $3 ... accel function
# $4 ... output file
run() {
    cpi="$1"
    ppi="$2"
    func="$3"
    outfile="$4"

    ./curves "any:?cpi=$1" "dummy:?ppi=$2&hz=60&bw=1024&bh=768" "$3:" > $outfile &

    pids="$pids $!"
}

# xorg is buggy somehow, skip it
funcs="osx windows"
cpis="400 1000 2000"
ppis="96 150 250"

filenames=""

for func in $funcs; do
    for cpi in $cpis; do
        for ppi in $ppis; do
            outfile="data-$func-cpi$cpi-ppi$ppi.dat"
            run "$cpi" "$ppi" "$func" "$outfile"
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
