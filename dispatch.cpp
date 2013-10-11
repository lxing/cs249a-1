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
  throw std::runtime_error("Failed to parse side");
}

// TODO: Handle Simulation exceptions
void dispatchLine(Simulation &simulation, char *textLine) {
  char *type = strtok(textLine, " ");
  if (!type || type[0] == '#') return;

  if (strcmp(type, "Tissue") == 0) {
    char *tissueName = strtok(NULL, " ");

    if (strcmp(tissueName, "tissueNew") == 0) {
      tissueName = strtok(NULL, " "); 
      simulation.tissueIs(tissueName);
      return;
    } 

    char *action = strtok(NULL, " ");

    if (strcmp(action, "infectedCellsDel") == 0) {
      simulation.infectionDel(tissueName);
      return;
    }

    if (strcmp(action, "cloneCellsNew") == 0) {
      simulation.clonedCellsAre(tissueName, parseSide());
      return;
    }

    Cell::Coordinates location = parseLocation();

    if (strcmp(action, "infectionStartLocationIs") == 0) {
      CellMembrane::Side side = parseSide();
      simulation.infectionIs(tissueName, location, side, parseStrength());
      return;
    }

    Cell::CellType cellType;
    if (strcmp(action, "cytotoxicCellNew") == 0) {
      cellType = Cell::cytotoxicCell_; 
    } else if (strcmp(action, "helperCellNew") == 0) {
      cellType = Cell::helperCell_; 
    } else {
      throw std::runtime_error("Failed to parse action");
    }

    try {
      simulation.cellIs(tissueName, cellType, location);
    } catch (...) {
      // Continue on
    }
  } else if (strcmp(type, "Cell") == 0) {
    char *tissueName = strtok(NULL, " ");
    Cell::Coordinates location = parseLocation();
    char *action = strtok(NULL, " ");
    CellMembrane::Side side = parseSide();

    if (strcmp(action, "cloneNew") == 0) {
      try {
        simulation.clonedCellIs(tissueName, location, side);
      } catch (...) {
        // Continue on
      }
    } else if (strcmp(action, "membrane") == 0) {
      strtok(NULL, " "); // Skip the superfluous "antibodyStrengthIs" directive
      simulation.antibodyStrengthIs(tissueName, location, side, parseStrength());
    } else {
      throw std::runtime_error("Failed to parse action");
    }
  }
}
