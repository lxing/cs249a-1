#include <fstream>
#include <stdexcept>
#include <stdlib.h>
#include "Simulation.h"
#include "Tissue.h"

using namespace std;

Cell::Coordinates parseLocation() {
  Cell::Coordinates location;
  location.x = atoi(strtok(NULL, " "));
  location.y = atoi(strtok(NULL, " "));
  location.z = atoi(strtok(NULL, " "));
  return location;
}

AntibodyStrength parseStrength() {
  AntibodyStrength strength(atoi(strtok(NULL, " ")));
  return strength;
}

CellMembrane::Side parseSide() {
  char *sideString = strtok(NULL, " ");
  if (strcmp(sideString, "north") == 0) return CellMembrane::north_;
  if (strcmp(sideString, "south") == 0) return CellMembrane::south_;
  if (strcmp(sideString, "east") == 0) return CellMembrane::east_;
  if (strcmp(sideString, "west") == 0) return CellMembrane::west_;
  if (strcmp(sideString, "up") == 0) return CellMembrane::up_;
  if (strcmp(sideString, "down") == 0) return CellMembrane::down_;
  throw std::runtime_error("Invalid side");
}

// TODO: Handle malformed input
void dispatchLine(const string textLineString, Simulation simulation) {
  char *textLine = strdup(textLineString.c_str());

  char *type = strtok(textLine, " ");
  if (!type || type[0] == '#') return;

  if (strcmp(type, "Tissue") == 0) {
    char *tissueName = strtok(NULL, " ");

    if (strcmp(tissueName, "tissueNew") == 0) {
      tissueName = strtok(NULL, " "); 
      simulation.TissueIs(tissueName);
      return;
    } 

    char *action = strtok(NULL, " ");

    if (strcmp(action, "infectedCellsDel") == 0) {
      simulation.InfectionDel(tissueName);
      return;
    }

    if (strcmp(action, "cloneCellsNew") == 0) {
      simulation.CloneCells(tissueName, parseSide());
      return;
    }

    Cell::Coordinates location = parseLocation();

    if (strcmp(action, "infectionStartLocationIs") == 0) {
      CellMembrane::Side side = parseSide();
      simulation.InfectionIs(tissueName, location, side, parseStrength());
      return;
    }

    Cell::CellType cellType;
    if (strcmp(action, "cytotoxicCellNew") == 0) {
      cellType = Cell::cytotoxicCell_; 
    } else if (strcmp(action, "helperCellNew") == 0) {
      cellType = Cell::helperCell_; 
    } else {
      throw std::runtime_error("Invalid action");
    }
    simulation.CellIs(tissueName, cellType, location);
  } else if (strcmp(type, "Cell") == 0) {
    char *tissueName = strtok(NULL, " ");
    Cell::Coordinates location = parseLocation();
    char *action = strtok(NULL, " ");
    CellMembrane::Side side = parseSide();

    if (strcmp(action, "cloneNew") == 0) {
      simulation.CloneCell(tissueName, location, side);
    } else if (strcmp(action, "membrane") == 0) {
      strtok(NULL, " "); // Skip the superfluous "antibodyStrengthIs" directive
      simulation.AntibodyStrengthIs(tissueName, location, side, parseStrength());
    } else {
      throw std::runtime_error("Invalid action");
    }
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
    dispatchLine(textLine, simulation);
  }

  return 0;
}
