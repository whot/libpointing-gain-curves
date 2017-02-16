Generated data by running the record-data.sh script and then moving the
mouse around a fair bit, trying to trigger the various speeds.
The script fired off a bunch of processes all with CPI400 on the osx curve
(darwin-16) but different speed settings. All were recording the same device
(and thus the same events) before sorting the data into a digestible format.

The plot-data.gnuplot script is a baseline to show the various curves.

How to interpret the plot:
Visible are the curves for the various speed settings. The data is a bit
messy but the difference of the lines is obvious enough. Input speed is in
mm/s of the physical mouse.

The actual gain value itself is a messy unit since it compares input speed
in mm/s to output speed in pixels (output/input), as provided by
libpointing. So a gain of 2 does *not* mean "twice as fast".

Side-note:
Any speed over 250mm/s is too fast to really expect anything useful to
happen (true for touchpad, haven't tried mouse yet). The xrange 1000 is just
to have the same visual format as the paper's graphs, but realistically the
right 75% are meaningless.
