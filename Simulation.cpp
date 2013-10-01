
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

void Simulation::CellIs(Cell::Coordinates _loc, Fwk::String tissue_name,
                         Cell::CellType _type) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(tissue_name);
  if (it != tissues_.end()) {
    // TODO(rhau) add some additional checks and validation
    printf("Tissue %s does not exist.\n", tissue_name.c_str());
    return;
  }

  Fwk::Ptr<Tissue> tissue = *it;
  Fwk::Ptr<Cell> cell = Cell::CellNew(_loc, tissue.ptr(), _type);
}

std::vector<Fwk::Ptr<Tissue> >::iterator Simulation::GetTissue(const Fwk::String _name) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it;
  for (it = tissues_.begin(); it != tissues_.end(); ++it) {
    Fwk::Ptr<Tissue> tissue = *it;
    if (strcmp(tissue->name().c_str(), _name.c_str()) == 0) {
      break;
    }
  }

  return it;
}