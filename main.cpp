#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <queue>
#include "Tissue.h"
#include "Simulation.h"

using namespace std;


bool extractLocation(std::istringstream& iss, Cell::Coordinates& _loc) {
  string x,y,z;
  if (!getline(iss, x, ' ')) return false;
  if (!getline(iss, y, ' ')) return false;
  if (!getline(iss, z, ' ')) return false;
  _loc.x = atoi(x[0] + "");
  _loc.y = atoi(y[0] + "");
  _loc.z = atoi(z[0] + "");
  return true;
} 

CellMembrane::Side extractSide(const string& side_str) {
  if (side_str.compare("north")==0) {
    return CellMembrane::north_;
  } else if (side_str.compare("east")==0) {
    return CellMembrane::east_;
  } else if (side_str.compare("south")==0) {
    return CellMembrane::south_;
  } else if (side_str.compare("west")==0) {
    return CellMembrane::west_;
  } else if (side_str.compare("up")==0) {
    return CellMembrane::up_;
  } else if (side_str.compare("down")==0) {
    return CellMembrane::down_;
  } else {
    // TODO throw exception
    throw "Invalid side supplied.";
  }
}

void executeLine(string line, Simulation simulation) {
  std::string token;
  std::istringstream iss(line);
  if (!getline(iss, token, ' ')) return;
  if (token[0] == '#') {
    // got comment
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
        throw "Unknown Cell New Exception";
        return;
      }
    }
  } else if (token.compare("Cell")==0) {
  // got cell
    string tissueName;
    if (!getline(iss, tissueName, ' ')) return; 
    Cell::Coordinates loc;
    if (!extractLocation(iss, loc)) {
      throw "Location not found exception";
      return;
    }

    string command;
    if (!getline(iss, command, ' ')==0) return; 
    if (command.compare("cloneNew")) {
      // clone command
      string side_str;
      if (!getline(iss, side_str, ' ')) return; 
      CellMembrane::Side side = extractSide(side_str);
      simulation.CloneCell(tissueName, loc, side);
      return;
    } else if (command.compare("membrane")==0) {
      // membrane command
      string side_str;
      if (!getline(iss, side_str, ' ')) return; 
      CellMembrane::Side side = extractSide(side_str);

      if (!getline(iss, command, ' ')) return; 
      if (command.compare("antibodyStrengthIs")==0) {
        string strength_str;
        if (!getline(iss, strength_str, ' ')) return;
        try {
          AntibodyStrength strength(atoi(strength_str.c_str()));  
          simulation.AntibodyStrengthIs(tissueName, loc, side, strength);
        } catch(...) {
          return;
        }
      }
      return;
    } else {
      throw "Unknown command exception";
      return;
    }

  } else {
    throw "Uknown command exception";
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
