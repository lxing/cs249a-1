#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <queue>
#include "Tissue.h"
#include "Simulation.h"

class SimulationDispatcher {
 public:
  static void DispatchLine(string line, Simulation* simulation);
  

 private:
  static void DispatchTissueCommand(std::istringstream& iss, Simulation* simulation);
  static void DispatchCellCommand(std::istringstream& iss, Simulation* simulation);

  static bool extractLocation(std::istringstream& iss, Cell::Coordinates& _loc);
  static CellMembrane::Side extractSide(const string& side_str);
};