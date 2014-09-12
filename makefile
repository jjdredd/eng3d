CXXOPTFLAGS = -O0 -march=native
GLFLAGS = -lglut -lGLU -lGL
CXXFLAGS = -W -Wextra -Wall --pedantic -g

main: main.cpp geometry.o bmp.o VisPhys.o
	g++ main.cpp geometry.o bmp.o VisPhys.o -o main \
	$(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)

geometry.o: geometry.cpp geometry.hpp bmp.hpp
	g++ geometry.cpp -c -o geometry.o $(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)

bmp.o: bmp.cpp bmp.hpp
	g++ bmp.cpp -c -o bmp.o $(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)

VisPhys.o: VisPhys.cpp VisPhys.hpp
	g++ VisPhys.cpp -c -o VisPhys.o $(CXXFLAGS) $(GLFLAGS) $(CXXOPTFLAGS)

clean:
	rm main
	rm geometry.o
	rm bmp.o
	VisPhys.o
