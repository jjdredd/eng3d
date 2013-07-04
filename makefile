CXXOPTFLAGS = -O3 -march=native
GLFLAGS = -lglut -lGLU -lGL
CXXFLAGS = -W -Wall --pedantic
main: main.cpp geometry.o bmp.o geometry.hpp
	g++ main.cpp geometry.o bmp.o -o main $(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)
geometry.o: geometry.cpp geometry.hpp bmp.hpp
	g++ geometry.cpp -c -o geometry.o $(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)
bmp.o: bmp.cpp bmp.hpp
	g++ bmp.cpp -c -o bmp.o $(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)
clean:
	rm main
	rm geometry.o
	rm bmp.o
