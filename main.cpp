#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <queue>
#include "Tissue.h"
#include "Simulation.h"

using namespace std;


private bool extractLocation(std::istringstream iss, Cell::Coordinates _loc) {
  string x,y,z;
  if (!getline(iss, x, ' ')) return false;
  if (!getline(iss, y, ' ')) return false;
  if (!getline(iss, z, ' ')) return false;
  _loc.x = atoi(x[0]);
  _loc.y = atoi(y[0]);
  _loc.z = atoi(z[0]);
  return true;
} 

private void executeLine(string line, Simulation simulation) {
  std::string token;
  std::istringstream iss(line);
  if (!getline(iss, token, ' ')) return;
  if (token[0] == '#') {
    // comment line
    return;
  }

  // got tissue
  if (token.compare("Tissue")==0) {
    // get command or tissue name
    if (!getline(iss, token, ' ')) return; 
    // got command
    if (token.compare("tissueNew")==0) { 
      // get tissue name
      if (!getline(iss, token, ' ')) return; 
      simulation.TissueIs(token);
      return;
    } else { 
    // got tissue name
      string tissueName = token;
      string command;
      if (!getline(iss, command, ' ')) return;

      if (command.compare("cytotoxicCellNew")) {

        return;
      } else if (command.compare("helperCellNew")) {
        return;
      } else if (command.compare("infectionStartLocationIs")) {
        return;
      } else if (command.compare("infectedCellsDel")) {
        return;
      } else if (command.compare("cloneCellsNew")) {
        return;
      } else {
        // TODO throw exception
        // return;
      }
    }
  } else if (token.compare("Cell")==0) {
  // got cell
    string tissueName;
    if (!getline(iss, tissueName, ' ')) return; 
    Cell::Coordinates loc;
    if (!extractLocation(&iss, &loc)) {
      // TODO throw exception
      return;
    }

    string command;
    if (!getline(iss, command, ' ')==0) return; 
    if (command.compare("cloneNew")) {
      // clone command
      string side;
      if (!getline(iss, side, ' ')) return; 
      simulation.CloneCell(tissueName, loc, side);
      return;
    } else if (command.compare("membrane")==0) {
      // membrane command
      string side;
      if (!getline(iss, side, ' ')) return; 
      if (!getline(iss, command, ' ')) return; 

      if (command.compare("antibodyStrengthIs")==0) {
        string strength_str;
        if (!getline(iss, strength_str, ' ')) return;
        try {
          AntibodyStrength strength(atoi(strength_str));  
          simulation.antibodyStrengthIs(tissueName, loc, side, strength);
        } catch(...) {
          return;
        }
      }
      return;
    } else {
      // TODO throw exception
      return;
    }

  } else {
  // TODO bad case throw exception
  }
}

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
    executeLine(line, simulation);
  }

  return 0;
}
