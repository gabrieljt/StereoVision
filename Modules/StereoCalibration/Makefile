all:
	if test -d Bin; then echo "Compiling..."; else mkdir Bin; fi
	g++ StereoCalibration.cpp `pkg-config --cflags --libs opencv` -o Bin/StereoCalibration
        
clean:
	rm Bin/StereoCalibration
	rmdir Bin
