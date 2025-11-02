
INCPATH := -L../libsgp4/ 

LIBPATH := -I../libsgp4/ 

LIBS = -lm -lsgp4s

all: visible_craft

visible_craft: visible_craft.cc
	$(CXX) $(CXXFLAGS) $(INCPATH) $^ -o $@ $(LIBPATH) $(LIBS)

clean: visible_craft
	rm -f visible_craft

beautify:
	astyle --options=../.astylerc *.cc
	rm -f *.orig
