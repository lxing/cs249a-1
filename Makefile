CPPFLAGS = -I.
CXXFLAGS = -Wall -g

OBJECTS = Simulation.o SimulationDispatcher.o Tissue.o main.o
LIBS = fwk/BaseCollection.o fwk/BaseNotifiee.o fwk/Exception.o

asgn1:	$(OBJECTS) $(LIBS)
	$(CXX) $(CXXFLAGS) -o asgn1 $(OBJECTS) $(LIBS)

clean:
	rm -f asgn1 $(OBJECTS) $(LIBS) *~

Simulation.o: Simulation.cpp Simulation.h
SimulationDispatcher.o: SimulationDispatcher.cpp SimulationDispatcher.h
Tissue.o: Tissue.cpp Tissue.h
main.o: main.cpp
