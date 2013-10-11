
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <queue>

#include "Tissue.h"
#include "Simulation.h"
#include "SimulationDispatcher.h"

using namespace std;

/*
  NOTE Worked with lxing on the main function, it's fairly boilerplate.
  The main takes in one input, the file name with the rules.
  The rules are then executed and the appropriate statistics are printed
  to the console.
*/
int main(int argc, const char* argv[]) {
  ifstream infile(argv[1]);
  if(infile.fail()){
    // File error. Halt program.
    cout << "error reading file" << endl;
    return 1;
  }

  Simulation simulation;

  // Read data in, parse it, excute commands.
  string line;
  while(!infile.eof()){
    getline(infile, line);
    SimulationDispatcher::DispatchLine(line, &simulation);
    // executeLine(line, simulation);
  }

  return 0;
}
