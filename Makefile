curves: curves.cpp
	g++ -o curves -lpointing -L/home/whot/code/libpointing/pointing -I/home/whot/code/libpointing/ curves.cpp


all: curves
