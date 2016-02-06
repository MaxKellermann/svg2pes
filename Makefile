SOURCES = src/Main.cxx

CXX = g++
CXXFLAGS = -std=c++11 -O0 -ggdb -fsanitize=address -Wall -Wextra -Werror
CPPFLAGS =
LDFLAGS =

all: svg2pes

clean:
	rm -f svg2pes src/*.o

svg2pes: $(SOURCES:.cxx=.o)
	$(CXX) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

%.o: %.cxx
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CPPFLAGS)
