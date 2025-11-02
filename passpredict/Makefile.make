
INCPATH := -L../libsgp4/

LIBPATH := -I../libsgp4/

LIBS = -lm -lsgp4s

all: passpredict

passpredict: passpredict.cc
	$(CXX) $(CXXFLAGS) $(INCPATH) $^ -o $@ $(LIBPATH) $(LIBS)

clean: passpredict
	rm -f passpredict

beautify:
	astyle --options=../.astylerc *.cc
	rm -f *.orig
