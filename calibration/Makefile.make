
INCPATH := -L../libsgp4/
LIBPATH := -I../libsgp4/

LIBS = -lm -lsgp4s

all: cochlear_scan

CXXFLAGS += -std=c++17

cochlear_scan: cochlear_scan.cc
	$(CXX) $(CXXFLAGS)  $(INCPATH) $^ -o $@ $(LIBPATH) $(LIBS)

clean:
	rm -f cochlear_scan

beautify:
	astyle --options=../.astylerc *.cc
	rm -f *.orig
