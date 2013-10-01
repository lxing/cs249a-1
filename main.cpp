#include <fstream>
#include <stdlib.h>
#include <queue>
#include "Tissue.h"
#include "Simulation.h"

using namespace std;

// TODO: Handle malformed input
void dispatchLine(string textLineString) {
  char *textLine = strdup(textLineString.c_str());

  char *token;
  token = strtok(textLine, " ");
  if (!token || token[0] == '#') return;
  char *type = strdup(token);
  char *tissueName = strdup(strtok(NULL, " "));

  Fwk::Ptr<Tissue> t;
  if (strcmp(tissueName, "tissueNew") != 0) {
    // Lookup tissue by name
  } else {
    t = Tissue::TissueNew(strtok(NULL, " "));
    // Register tissue in simulation
    return;
  }

  if (strcmp(type, "Tissue") == 0) {

  } else if (strcmp(token, "Cell") == 0) {

  }
}
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
    dispatchLine(textLine);
  }

  return 0;
}