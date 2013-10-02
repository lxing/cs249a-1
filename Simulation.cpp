
#include <fstream>
#include <stdlib.h>
#include "Simulation.h"

Simulation::Simulation() {
  // TODO(rhau) fill in
}

Simulation::~Simulation() {
  // TODO(rhau) fill in
}

void Simulation::TissueIs (const Fwk::String _name) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(_name);
  if (it != tissues_.end()) {
    // TODO(rhau) add some additional checks and validation
    printf("Duplicate tissues: %s\n", _name.c_str());
    return;
  }

  Fwk::Ptr<Tissue> ptr(Tissue::TissueNew(_name));
  tissues_.push_back(ptr);
}

void Simulation::CellIs (Fwk::String _tissueName, Cell::CellType _type, 
                         Cell::Coordinates _loc) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(_tissueName);
  if (it != tissues_.end()) {
    // TODO(rhau) add some additional checks and validation
    printf("Tissue %s does not exist.\n", _tissueName.c_str());
    return;
  }

  Fwk::Ptr<Tissue> tissue = *it;
  Fwk::Ptr<Cell> cell = Cell::CellNew(_loc, tissue.ptr(), _type);
}

void Simulation::InfectionIs(Fwk::String tissueName_, Cell::Coordinates _loc,
                             CellMembrane::Side _side, AntibodyStrength _strength) {
  // TODO(rhau) fill in this method
}

void Simulation::InfectedCellsDel(Fwk::String _tissueName) {
  // TODO(rhau) fill in this method
}


void Simulation::CloneCell (Fwk::String _tissueName, Cell::Coordinates _loc,
                            CellMembrane::Side _side) {
  // TODO(lxing) fill in this method
}

void Simulation::CloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
  // TODO(lxing) fill in this method
}

void Simulation::AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
                                     CellMembrane::Side _side, AntibodyStrength _strength) {
  // TODO(lxing) fill in this method
}

std::vector<Fwk::Ptr<Tissue> >::iterator Simulation::GetTissue(
    const Fwk::String _name) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it;
  for (it = tissues_.begin(); it != tissues_.end(); ++it) {
    Fwk::Ptr<Tissue> tissue = *it;
    if (strcmp(tissue->name().c_str(), _name.c_str()) == 0) {
      break;
    }
  }
  return it;
}

