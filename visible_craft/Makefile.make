
INCPATH := -L../libsgp4/ 

LIBPATH := -I../libsgp4/ 

LIBS = -lm -lsgp4s

all: visible_craft look_angle_generator

visible_craft: visible_craft.cc
	$(CXX) $(CXXFLAGS) $(INCPATH) $^ -o $@ $(LIBPATH) $(LIBS)

look_angle_generator: look_angle_generator.cc
	$(CXX) $(CXXFLAGS) $(INCPATH) $^ -o $@ $(LIBPATH) $(LIBS)

clean:
	rm -f visible_craft look_angle_generator

beautify:
	astyle --options=../.astylerc *.cc
	rm -f *.orig
