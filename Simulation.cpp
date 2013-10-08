
#include <assert.h>
#include <fstream>
#include <queue>
#include <stdlib.h>
#include "Simulation.h"

#define kMembraneName "Membrane"

void Simulation::TissueReactor::onCellNew(Cell::Ptr _cell) {
  for (int i=0; i<6; i++) {
    CellMembrane::Side side = static_cast<CellMembrane::Side>(i);
    AntibodyStrength strength(0);
    if (_cell->cellType() == Cell::cytotoxicCell_) {
      strength.valueIs(99);
    } else if (_cell->cellType() == Cell::helperCell_) {
      strength.valueIs(0);
    } else {
      // TODO(rhau) throw exception if not one of those cell types
    }

    // create membrane
    CellMembrane::Ptr membrane = _cell->membraneNew(kMembraneName, side);
    // update strength
    _cell->membrane(side)->antibodyStrengthIs(strength); 
  }
}

Simulation::SimulationStats::SimulationStats() : 
    numInfectedCells(0), numInfectionAttempts(0),
    totalDiseaseAndAntibodyStrengthDiff(0),
    numLiveCytotoxicCells(0), numLiveHelperCells(0),
    infectionSpread(0), longestInfectionPathLength(0) {
}

Simulation::Simulation() {
  // TODO(rhau) fill in
}

Simulation::~Simulation() {
  // TODO(rhau) fill in
}

void Simulation::TissueIs (const Fwk::String _name) {
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_name);
  if (it != tissues_.end()) return;

  Tissue::Ptr tissue(Tissue::TissueNew(_name));
  TissueReactor *m = TissueReactor::TissueReactorIs(tissue.ptr());
  tissues_.push_back(tissue);
}

void Simulation::CellIs (Fwk::String _tissueName, Cell::CellType _type, 
                         Cell::Coordinates _loc) {
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  Tissue::Ptr tissue = *it;
  Cell::Ptr cell = Cell::CellNew(_loc, tissue.ptr(), _type);
  tissue->cellIs(cell); //TODO(rhau) throw exception if this fails
}


bool Simulation::InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                                AntibodyStrength _strength) {
  if (_cell->health() == _cell->infected()) return false;
  CellMembrane::Ptr membrane = _cell->membrane(_side);
  if (_strength <= membrane->antibodyStrength()) return false; 
  _cell->healthIs(_cell->infected());
  return true;
}

void Simulation::InfectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                             CellMembrane::Side _side, AntibodyStrength _strength) {
  std::queue<Cell::Ptr> infectionFringe;
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
  CheckTissue(it);
  Tissue::Ptr tissue = (*it);

  Cell::Ptr cell = tissue->cell(_loc);
  if (!InfectedCellIs(cell, _side, _strength)) return;
  infectionFringe.push(cell);

  while (!infectionFringe.empty()) {
    cell = infectionFringe.front(); 

    infectionFringe.pop();
  }
}

void Simulation::InfectedCellsDel(Fwk::String _tissueName) {
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
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
  if (!cell) {
    // TODO(rhau) throw exception;
    printf("Cell to clone is null.\n");
    return;
  }

  Cell::Coordinates clone_loc = GetCloneLocation(_loc, _side);
  Cell::PtrConst existing_cell = (*it)->cell(clone_loc);
  if (existing_cell) {
    // TODO(rhau) throw exception;
    printf("Cell already exists in clone location.\n");
    return; 
  }

  CellIs((*it)->name(), cell->cellType(), clone_loc);
}

void Simulation::CloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  // TODO(lxing) fwkHmNext instead of raw iterator
  vector<Cell::Coordinates> clone_locations;
  Tissue::CellIteratorConst cell_iter = (*it)->cellIterConst();
  for (; cell_iter != NULL; ++cell_iter) {
    clone_locations.push_back((*cell_iter)->location());
  }

  int i;
  for (; i<clone_locations.size(); i++) {
    Cell::Coordinates coord = clone_locations[i];
    CloneCell(_tissueName, coord, _side);        
  }
}

void Simulation::AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
                                     CellMembrane::Side _side, AntibodyStrength _strength) {
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  Cell::Ptr cell = (*it)->cell(_loc);
  // TODO(rhau) verify that cell exists
  CellMembrane::Ptr membrane = cell->membrane(_side);
  membrane->antibodyStrengthIs(_strength);
}

std::vector<Tissue::Ptr>::iterator Simulation::GetTissue(
    const Fwk::String _name) {
  std::vector<Tissue::Ptr>::iterator it;
  for (it = tissues_.begin(); it != tissues_.end(); ++it) {
    Tissue::Ptr tissue = *it;
    if (strcmp(tissue->name().c_str(), _name.c_str()) == 0) {
      break;
    }
  }
  return it;
}

void Simulation::CheckTissue(
    const std::vector<Tissue::Ptr>::iterator it) {
  if (it == tissues_.end()) {
    // TODO(rhau) add some additional checks and validation
    printf("Tissue did not exist.\n");
    // TODO(rhau) change the asserts to throw exceptions instead
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
