#include <assert.h>
#include <stdexcept>
#include <stdlib.h>
#include "Simulation.h"

#define SIDE_MIN 0
#define SIDE_MAX 6
#define MEMBRANE_NAME "Membrane"

/*******************/
/* Simulation Impl */
/*******************/

Simulation::Simulation() {}
Simulation::~Simulation() {}

void Simulation::tissueIs(Fwk::String _tissueName) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (tissue.ptr()) return;
  Tissue::Ptr newTissue(Tissue::TissueNew(_tissueName));   
  tissues_.push_back(newTissue); 

  TissueReactor::Ptr reactor = TissueReactor::TissueReactorNew(newTissue);
  reactors_.push_back(reactor);
}

void Simulation::cellIs(Fwk::String _tissueName, Cell::CellType _type, 
                        Cell::Coordinates _loc) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::PtrConst existing = tissue->cell(_loc);
  if (existing.ptr()) throw std::runtime_error("Cell Overwrite");
  Cell::Ptr cell = Cell::CellNew(_loc, tissue.ptr(), _type);
  tissue->cellIs(cell);
}

bool Simulation::infectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                                AntibodyStrength _strength) {
  return true;
}


void Simulation::infectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                             CellMembrane::Side _side, AntibodyStrength _strength) {
}

void Simulation::infectionDel(Fwk::String _tissueName) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;

  for (Tissue::CellIterator it = tissue->cellIter(); it; ++it) {
    if ((*it)->health() == Cell::infected_) {
      tissue->cellDel((*it)->location().name());
    }
  }
}

void Simulation::cloneCell(Fwk::String _tissueName, Cell::Coordinates _loc,
                           CellMembrane::Side _side) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::PtrConst cell = tissue->cell(_loc);
  if (!cell.ptr()) return;

  Cell::Coordinates newLoc = locationMove(_loc, _side);
  cellIs(_tissueName, cell->cellType(), newLoc);

  // Copy over antibody strengths
  Cell::Ptr newCell = tissue->cell(newLoc);
  for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
    CellMembrane::Side side = (CellMembrane::Side) i;
    newCell->membrane(side)->antibodyStrengthIs(
      cell->membrane(side)->antibodyStrength());
  }
}

void Simulation::cloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;

  vector<Cell::Coordinates> cloneLocs;
  for (Tissue::CellIteratorConst it = tissue->cellIterConst(); it; ++it) {
    cloneLocs.push_back((*it)->location());
  }

  for (U32 i = 0; i < cloneLocs.size(); ++i) {
    try {
      cloneCell(_tissueName, cloneLocs[i], _side);
    } catch (...) {
      // No-op, just continue cloning
    }
  }
}

void Simulation::antibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
                                     CellMembrane::Side _side, AntibodyStrength _strength) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::Ptr cell = tissue->cell(_loc);
  if (!cell.ptr()) return;
  CellMembrane::Ptr membrane = cell->membrane(_side);
  if (!membrane.ptr()) return;
  membrane->antibodyStrengthIs(_strength); 
}


/*****************/
/* TissueReactor */
/*****************/

void Simulation::TissueReactor::onCellNew(Cell::Ptr _cell) {
  AntibodyStrength strength;
  switch (_cell->cellType()) {
    case Cell::cytotoxicCell_ : strength.valueIs(100); break;
    case Cell::helperCell_ : strength.valueIs(0); break;
    default : throw std::runtime_error("Invalid cell type");
  }

  // Break abstraction to manually iterate through sides
  for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
    CellMembrane::Side side = (CellMembrane::Side) i;
    CellMembrane::Ptr membrane = _cell->membraneNew(MEMBRANE_NAME, side);
    membrane->antibodyStrengthIs(strength); 
  }
}


/**********************/
/* Simulation Helpers */
/**********************/

Tissue::Ptr Simulation::tissue(Fwk::String _tissueName) {
  Tissue::Ptr tissue;
  for (TissueList::iterator it = tissues_.begin(); it != tissues_.end(); ++it) {
    tissue = *it;
    if (!strcmp(tissue->name().c_str(), _tissueName.c_str()))
      break;
  }
  return tissue;
}

Simulation::TissueReactor::Ptr Simulation::reactor(Fwk::String _tissueName) {
  TissueReactor::Ptr reactor;
  for (ReactorList::iterator it = reactors_.begin(); it != reactors_.end(); ++it) {
    reactor = *it;
    if (!strcmp(reactor->tissue()->name().c_str(), _tissueName.c_str()))
      break;
  }
  return reactor;
}

Cell::Coordinates Simulation::locationMove(Cell::Coordinates _loc, CellMembrane::Side _side) {
  Cell::Coordinates newLoc = _loc;
  switch (_side) {
    case CellMembrane::north_ : newLoc.y += 1; break;
    case CellMembrane::south_: newLoc.y -= 1; break;
    case CellMembrane::east_: newLoc.x += 1; break;
    case CellMembrane::west_: newLoc.x -= 1; break;
    case CellMembrane::up_: newLoc.z += 1; break;
    case CellMembrane::down_: newLoc.z -= 1; break;
    default : throw std::runtime_error("Invalid side");
  }
  return newLoc;
}
