
#include <assert.h>
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
  if (it != NULL) return;
  Fwk::Ptr<Tissue> ptr(Tissue::TissueNew(_name));
  tissues_.push_back(ptr);
}

void Simulation::CellIs (Fwk::String _tissueName, Cell::CellType _type, 
                         Cell::Coordinates _loc) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  Fwk::Ptr<Tissue> tissue = *it;
  Fwk::Ptr<Cell> cell = Cell::CellNew(_loc, tissue.ptr(), _type);
  tissue->cellIs(cell);
  assert(tissue->cells()==U32(1));
}

void Simulation::InfectionIs(Fwk::String tissueName_, Cell::Coordinates _loc,
                             CellMembrane::Side _side, AntibodyStrength _strength) {
  // TODO(rhau) fill in this method
}

void Simulation::InfectedCellsDel(Fwk::String _tissueName) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  // TODO(lxing) fwkHmNext instead of raw iterator
  Tissue::CellIteratorConst cell_iter = (*it)->cellIterConst();
  for ( ; cell_iter != NULL; ++cell_iter) {
    (*it)->cellDel((*cell_iter)->location().name());
  }
}


void Simulation::CloneCell (Fwk::String _tissueName, Cell::Coordinates _loc,
                            CellMembrane::Side _side) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  Cell::PtrConst cell = (*it)->cell(_loc);
  // TODO(rhau) CheckCell();
  Cell::Coordinates clone_loc = GetCloneLocation(_loc, _side);
  // TODO(rhau) CheckLoc();

  CellIs((*it)->name(), cell->cellType(), clone_loc);
}

void Simulation::CloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
  // TODO(lxing) fill in this method
}

void Simulation::AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
                                     CellMembrane::Side _side, AntibodyStrength _strength) {
  std::vector<Fwk::Ptr<Tissue> >::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  Cell::Ptr cell = (*it)->cell(_loc);
  CellMembrane::Ptr membrane = cell->membrane(_side);
  membrane->antibodyStrengthIs(_strength);
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

void Simulation::CheckTissue(
    const std::vector<Fwk::Ptr<Tissue> >::iterator it) {
  if (it == tissues_.end()) {
    // TODO(rhau) add some additional checks and validation
    printf("Tissue did not exist.\n");
    assert(it != tissues_.end());
    return;
  }
}

Cell::Coordinates Simulation::GetCloneLocation(Cell::Coordinates _loc, CellMembrane::Side _side) {
  Cell::Coordinates clone_loc = _loc;
  if (_side == CellMembrane::north_) {
    clone_loc.y += 1;
  } else if (_side == CellMembrane::south_) {
    clone_loc.y -= 1;
  } else if (_side == CellMembrane::east_) {
    clone_loc.x += 1;
  } else if (_side == CellMembrane::west_) {
    clone_loc.x -= 1;
  } else if (_side == CellMembrane::up_) {
    clone_loc.z += 1;
  } else if (_side == CellMembrane::down_) {
    clone_loc.z -= 1;
  } else {
    // TODO(rhau) throw exceptioin
  }

  return clone_loc;
}
