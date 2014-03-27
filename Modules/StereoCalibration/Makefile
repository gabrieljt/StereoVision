all:
	if test -d Bin; then echo "Compiling..."; else mkdir Bin; fi
	g++ StereoCalibrate.cpp `pkg-config --cflags --libs opencv` -o Bin/StereoCalibrate
        
clean:
	rm Bin/StereoCalibrate
	rmdir Bin
