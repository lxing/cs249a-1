#include "SimulationDispatcher.h"

void SimulationDispatcher::DispatchLine(string line, Simulation* simulation) {
  std::string token;
  std::istringstream iss(line);
  if (!getline(iss, token, ' ')) return;
  if (token[0] == '#') {
    // got comment
    return;
  }

  // got tissue
  if (token.compare("Tissue")==0) {
    DispatchTissueCommand(iss, simulation);
    return;
  }

  if (token.compare("Cell")==0) {
    DispatchCellCommand(iss, simulation);
  }
}

void SimulationDispatcher::DispatchTissueCommand(std::istringstream& iss,
   Simulation* simulation) {
  std::string token;
  if (!getline(iss, token, ' ')) return;

  if (token.compare("tissueNew")==0) {
    std::string tissue_name;
    if (!getline(iss, tissue_name, ' ')) return;
    simulation->TissueIs(tissue_name);
    return;
  }

  std::string tissue_name = token;
  std::string command;
  if (!getline(iss, command, ' ')) return;
  if (command.compare("infectedCellsDel")==0) {
    simulation->InfectedCellsDel(tissue_name);
    return;
  }

  if (command.compare("cloneCellsNew")==0) {
    std::string side_str;
    if (!getline(iss, side_str, ' ')) return;
    try {
      simulation->CloneCells(tissue_name, extractSide(side_str));
      return;
    } catch (...) {
      return;
    }
  }

  Cell::Coordinates loc;
  if (!extractLocation(iss, loc)) return;
  if (command.compare("cytotoxicCellNew")==0) {
    simulation->CellIs(tissue_name, Cell::cytotoxicCell_, loc);
    return;
  }
  
  if (command.compare("helperCellNew")==0) {
    simulation->CellIs(tissue_name, Cell::helperCell_, loc);
    return;
  }

  if (command.compare("infectionStartLocationIs")==0) {
    std::string side_str;
    if (!getline(iss, side_str, ' ')) return;
    try {
      string strength_str;
      if (!getline(iss, strength_str, ' ')) return;
      simulation->InfectionIs(tissue_name, loc, extractSide(side_str),
        AntibodyStrength(atoi(strength_str.c_str())));
    } catch (...) { }
    return;
  }
}

void SimulationDispatcher::DispatchCellCommand(std::istringstream& iss,
   Simulation* simulation) {
  std::string tissue_name;
  if (!getline(iss, tissue_name, ' ')) return;

  Cell::Coordinates loc;
  if (!extractLocation(iss, loc)) return;

  std::string command;
  if (!getline(iss, command, ' ')) return;

  std::string side_str;
  if (!getline(iss, side_str, ' ')) return;
  
  CellMembrane::Side side;
  try { side = extractSide(side_str); }
  catch (...) { return; }

  if (command.compare("cloneNew")==0) {
    simulation->CloneCell(tissue_name, loc, side);
    return;
  }

  if (command.compare("membrane")==0) {
    string subcommand;
    if (!getline(iss, subcommand, ' ')) return;
    if (subcommand.compare("antibodyStrengthIs")==0) {
      string strength_str;
      if (!getline(iss, strength_str, ' ')) return;
      try {
        AntibodyStrength strength(atoi(strength_str.c_str()));  
        simulation->AntibodyStrengthIs(tissue_name, loc, side, strength);
      } catch(...) { }
      return;  
    }
  }
}

bool SimulationDispatcher::extractLocation(std::istringstream& iss,
    Cell::Coordinates& _loc) {
  string x,y,z;
  if (!getline(iss, x, ' ')) return false;
  if (!getline(iss, y, ' ')) return false;
  if (!getline(iss, z, ' ')) return false;
  _loc.x = atoi(x.c_str());
  _loc.y = atoi(y.c_str());
  _loc.z = atoi(z.c_str());
  return true;
}

CellMembrane::Side SimulationDispatcher::extractSide(const string& side_str) {
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
    // TODO catch exception
    throw "Invalid side supplied.";
  }
}