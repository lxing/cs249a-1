#include <fstream>
#include <stdlib.h>
#include "dispatch.cpp"
#include "Simulation.h"
#include "Tissue.h"

/*
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
  string textLine;
  while(!infile.eof()){
    getline(infile, textLine);
    char *textLineC = strdup(textLine.c_str());
    dispatchLine(simulation, textLineC);
    free(textLineC);
  }

  return 0;
}
