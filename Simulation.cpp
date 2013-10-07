#include <assert.h>
#include <stdexcept>
#include <stdlib.h>
#include "Simulation.h"

Simulation::Simulation() {}
Simulation::~Simulation() {}

void Simulation::TissueIs(Fwk::String _tissueName) {
  Tissue::Ptr tissue = GetTissue(_tissueName);
  if (tissue.ptr()) return;
  cout << _tissueName;
  Tissue::Ptr tissueNew(Tissue::TissueNew(_tissueName));   
  tissues_.push_back(tissueNew); 
}

void Simulation::CellIs (Fwk::String _tissueName, Cell::CellType _type, 
                         Cell::Coordinates _loc) {
  Tissue::Ptr tissue = GetTissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::PtrConst existing = tissue->cell(_loc);
  if (existing.ptr()) throw std::runtime_error("Cell Overwrite");
  Cell::Ptr cell = Cell::CellNew(_loc, tissue.ptr(), _type);
  tissue->cellIs(cell);
}

bool Simulation::InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                                AntibodyStrength _strength) {
  return true;
}


void Simulation::InfectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                             CellMembrane::Side _side, AntibodyStrength _strength) {
}

void Simulation::InfectionDel(Fwk::String _tissueName) {
  Tissue::Ptr tissue = GetTissue(_tissueName);
  if (!tissue.ptr()) return;

  vector<Cell::Coordinates> infectedLocs;
  for (Tissue::CellIteratorConst it = tissue->cellIterConst(); it; ++it) {
    if ((*it)->healthIs(Cell::infected())) {
      tissue->cellDel((*it)->location().name());
    }
  }
}

void Simulation::CloneCell(Fwk::String _tissueName, Cell::Coordinates _loc,
                           CellMembrane::Side _side) {
  Tissue::Ptr tissue = GetTissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::PtrConst cell = tissue->cell(_loc);
  if (!cell.ptr()) return;

  Cell::Coordinates newLoc = LocationMove(_loc, _side);
  CellIs(_tissueName, cell->cellType(), newLoc);

  // TODO: Copy antibody strengths
}

void Simulation::CloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
  Tissue::Ptr tissue = GetTissue(_tissueName);
  if (!tissue.ptr()) return;

  vector<Cell::Coordinates> cloneLocs;
  for (Tissue::CellIteratorConst it = tissue->cellIterConst(); it; ++it) {
    cloneLocs.push_back((*it)->location());
  }

  for (U8 i = 0; i < cloneLocs.size(); ++i) {
    try {
      CloneCell(_tissueName, cloneLocs[i], _side);
    } catch (...) {
      // No-op, just continue cloning
    }
  }
}

void Simulation::AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
                                     CellMembrane::Side _side, AntibodyStrength _strength) {
  Tissue::Ptr tissue = GetTissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::Ptr cell = tissue->cell(_loc);
  if (!cell.ptr()) return;
  CellMembrane::Ptr membrane = cell->membrane(_side);
  if (!membrane.ptr()) return;
  membrane->antibodyStrengthIs(_strength); 
}

Tissue::Ptr Simulation::GetTissue(Fwk::String _tissueName) {
  Tissue::Ptr tissue;
  for (std::vector<Tissue::Ptr>::iterator it = tissues_.begin(); it != tissues_.end(); ++it) {
    tissue = *it;
    if (!strcmp(tissue->name().c_str(), _tissueName.c_str())) {
      return tissue;
    }
  }
  return tissue;
}

Cell::Coordinates Simulation::LocationMove(Cell::Coordinates _loc, CellMembrane::Side _side) {
  Cell::Coordinates newLoc = _loc;
  switch (_side) {
    case CellMembrane::north_ : newLoc.y += 1;
    case CellMembrane::south_: newLoc.y -= 1;
    case CellMembrane::east_: newLoc.x += 1;
    case CellMembrane::west_: newLoc.x -= 1;
    case CellMembrane::up_: newLoc.z += 1;
    case CellMembrane::down_: newLoc.z -= 1;
  }
  return newLoc;
}
