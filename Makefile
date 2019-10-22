all: EightPointAlgorithm.cpp main.cpp
	@echo "Building Eight-Point Algorithm"
	@if [ -d "bin" ]; then rm -r bin; fi
	@mkdir bin
	#g++ -I /usr/local/include/eigen3/ EightPointAlgorithm.cpp main.cpp -o bin/test -lopencv_core -lopencv_calib3d
	g++ EightPointAlgorithm.cpp main.cpp -o bin/test -lopencv_core -lopencv_calib3d
	@echo "Build finished"
	@echo "Run ./bin/test for the example"
	