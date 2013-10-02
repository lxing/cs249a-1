CPPFLAGS = -I.
CXXFLAGS = -Wall -g

OBJECTS = Simulation.o Tissue.o main.o
LIBS = fwk/BaseCollection.o fwk/BaseNotifiee.o fwk/Exception.o

asgn1:	$(OBJECTS) $(LIBS)
	$(CXX) $(CXXFLAGS) -o asgn1 $(OBJECTS) $(LIBS)

clean:
	rm -f asgn1 $(OBJECTS) $(LIBS) *~

Simulation.o: Simulation.cpp Simulation.h
Tissue.o: Tissue.cpp Tissue.h
main.o: main.cpp
