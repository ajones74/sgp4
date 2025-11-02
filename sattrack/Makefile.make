
INCPATH := -L../libsgp4/ 

LIBPATH := -I../libsgp4/ 

LIBS = -lm -lsgp4s

all: sattrack

sattrack: sattrack.cc
	$(CXX) $(CXXFLAGS) $(INCPATH) $^ -o $@ $(LIBPATH) $(LIBS)

clean: sattrack
	rm -f sattrack

beautify:
	astyle --options=../.astylerc *.cc
	rm -f *.orig
