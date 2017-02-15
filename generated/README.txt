Generated data by running the record-data.sh script and then moving the
mouse around a fair bit, trying to trigger the various speeds.
The script fired off a bunch of processes with different arguments,
recording the same device (and thus the same events) before sorting the
data into a digestible format.

The plot-data.gnuplot script is a baseline to show the various curves.

Note: the ppi value makes no difference, so only one ppi value was tested.
Note: the CPI value scales the curve but for ease-of-interpretation only
three values 400/1000/2000 are used, you get the gist from that anyway.

How to interpret the plot:
Visible are six curves, three for osx, three for windows. The data is a bit
messy but the difference of the lines is obvious enough. The windows accel
curve has a double-hump (see the No More Bricolage paper [1]). The OSX curve
is more of a half-bell curve but that's not overly visible here. Note that
this curve is closer to the OSX touchpad curve in the paper than the OSX
mouse curve, darwin-15 changed that, re-running with darwin-10 or darwin-14
data would produce the same as in the paper.

The actual gain value itself is a messy unit since it compares input speed
in mm/s to output speed in pixels, as provided by libpointing. So a gain of
2 does *not* mean "twice as fast".

What *is* visible is that the CPI count makes a difference. The data uses
normalized speed in mm/s (taking the CPI into account), i.e. devices of
different CPI have the correct mm/s speed calculated.

For a transfer function that knows about the CPI, the curves should be the
same for all devices, i.e. translating the same input speed to the same
gain and thus the same normalized pixel output.

Because the accel functions themselves don't appear to take CPI into
account, higher-resolution devices get a higher gain (proportional to the
400CPI baseline). At the same movement speed, a 800CPI device generates
deltas twice as big as a 400CPI device - any transfer function that ignores
CPI will thus think the device is twice as fast and accelerate accordingly.

Something else is going on here though: CPI 1000 at 200mm/s should be half
the gain of CPI 2000 at 200mm/s. That is not the case though, the scaling
appears to be larger than that.

Side-note:
Any speed over 250mm/s is too fast to really expect anything
useful to happen (true for touchpad, haven't tried mouse yet). The xrange
1000 is just to have the same visual format as the paper's graphs, but
realistically the right 75% are meaningless.

[1] http://mjolnir.lille.inria.fr/~roussel/publications/2011-UIST-libpointing.pdf
