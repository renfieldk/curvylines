all: curvylines.exe

curvylines.exe: curvylines.o
	g++ -o curvylines.exe curvylines.o -F/Library/Frameworks/SDL2.framework/ -framework SDL2

curvylines.o: curvylines.cpp
	g++ --std=c++0x -c curvylines.cpp

clean:
	rm curvylines.o curvylines.exe

