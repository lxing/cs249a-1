
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
      if (i==0) owner_->incNumLiveCytotoxicCells();
      strength.valueIs(100);
    } else if (_cell->cellType() == Cell::helperCell_) {
      if (i==0)  owner_->incNumLiveHelperCells();
      strength.valueIs(0);
    } else {
      printf("cell type did not exist\n");
      throw "Cell type does not exist";
    }

    // create membrane
    CellMembrane::Ptr membrane = _cell->membraneNew(kMembraneName, side);
    // update strength
    _cell->membrane(side)->antibodyStrengthIs(strength); 
  }
}

Simulation::SimulationStats::SimulationStats() : 
    numInfectedCells_(0), numInfectionAttempts_(0),
    totalDiseaseAndAntibodyStrengthDiff_(0),
    numLiveCytotoxicCells_(0), numLiveHelperCells_(0),
    infectionSpread_(0), longestInfectionPathLength_(0) {
}

string Simulation::SimulationStats::ToString() {
  CalculateInfectionSpread();

  // TODO substitute letters in if values never set
  std::stringstream ss;
  ss << numInfectedCells_ << " " // a
     << numInfectionAttempts_ << " "  // b
     << totalDiseaseAndAntibodyStrengthDiff_ << " " // c
     << numLiveCytotoxicCells_ << " " // d
     << numLiveHelperCells_ << " " // e
     << infectionSpread_ << " " // f
     << longestInfectionPathLength_; // g
  return ss.str();
}

void Simulation::SimulationStats::UpdateSpread(Cell::Coordinates _loc) {
  if (_loc.y > north_loc_.y) north_loc_ = _loc;
  if (_loc.y < south_loc_.y) south_loc_ = _loc;

  if (_loc.y > east_loc_.x) east_loc_ = _loc;
  if (_loc.y < west_loc_.y) west_loc_ = _loc;

  if (_loc.y > top_loc_.z) top_loc_ = _loc;
  if (_loc.y < north_loc_.z) bottom_loc_ = _loc;
}

void Simulation::SimulationStats::UpdatePathLength(Cell::Coordinates _loc) {
  U64 distance = abs(_loc.x - root_loc_.x)
    + abs(_loc.y - root_loc_.y)
    + abs(_loc.z - root_loc_.z);
  longestInfectionPathLength_ = distance;
}

void Simulation::SimulationStats::CalculateInfectionSpread() {
  U64 height = abs(north_loc_.y - south_loc_.y) + 1;
  U64 width = abs(east_loc_.x - west_loc_.x) + 1;
  U64 depth = abs(top_loc_.z - bottom_loc_.z) + 1;
  infectionSpread_ = height * width * depth;
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
  TissueReactor *m = TissueReactor::TissueReactorIs(tissue.ptr(), this);
  tissues_.push_back(tissue);
  tissue_ptrs_.push_back(tissue.ptr());
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
  if (_cell->health() == _cell->infected()) {
    // TODO check if infection attempt includes an already infected cell
    return false;
  }
  CellMembrane::Ptr membrane = _cell->membrane(_side);
  if (_strength <= membrane->antibodyStrength()) {
    stats_.incNumInfectionAttempts();
    return false; 
  }
  _cell->healthIs(_cell->infected());
  stats_.incNumInfectedCells();
  stats_.UpdateSpread(_cell->location());
  stats_.UpdatePathLength(_cell->location());
  return true;
}

void Simulation::InfectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
    CellMembrane::Side _side, AntibodyStrength _strength) {
  stats_.RootLocIs(_loc);
  
  std::queue<Cell::Ptr> infectionFringe1;
  std::queue<Cell::Ptr> infectionFringe2;
  std::queue<Cell::Ptr>* currFringe = &infectionFringe1;
  std::queue<Cell::Ptr>* nextFringe = &infectionFringe2;

  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
  CheckTissue(it);
  Tissue::Ptr tissue = (*it);

  Cell::Ptr cell = tissue->cell(_loc);
  if (!cell.ptr() || !InfectedCellIs(cell, _side, _strength)) return;
  currFringe->push(cell);

  while (!(currFringe->empty() && nextFringe->empty())) {
    stats_.incLongestInfectionPathLength();
    while (!currFringe->empty()) {
      cell = currFringe->front(); 

      for (int i=0; i<6; i++) {
        CellMembrane::Side side = static_cast<CellMembrane::Side>(i);
        Cell::Coordinates next_loc = GetCellLocation(cell->location(), side);
        Cell::Ptr next_cell = tissue->cell(next_loc);
        if (next_cell.ptr() != NULL &&
            next_cell->health() != Cell::infected_ &&
            InfectedCellIs(next_cell, oppositeSide(side), _strength)) {
          nextFringe->push(next_cell);
        }
      }
      currFringe->pop();
    }

    swap(*currFringe, *nextFringe);
  }

  // print out stats after each round
  cout << stats_.ToString() << endl;
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
    cout <<"Cell to clone is null." << endl;
    throw "Null cell exception";
    return;
  }

  Cell::Coordinates clone_loc = GetCellLocation(_loc, _side);
  Cell::PtrConst existing_cell = (*it)->cell(clone_loc);
  if (existing_cell) {
    printf("Cell already exists in clone location.\n");
    throw "Existing cell exception";
    return; 
  }

  CellIs((*it)->name(), cell->cellType(), clone_loc);
  Cell::Ptr cloned_cell = (*it)->cell(clone_loc);
  if (!cell) {
    cout <<"Cell clone failed." << endl;
    throw "Null cloned cell exception";
    return;
  }

  // update membrane strengths
  for (int i=0; i<6; i++) {
    CellMembrane::Side side = static_cast<CellMembrane::Side>(i);
    AntibodyStrength strength = cell->membrane(side)->antibodyStrength();
    CellMembrane::Ptr membrane = cloned_cell->membrane(side);
    membrane->antibodyStrengthIs(strength); 
  }
}

void Simulation::CloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
  std::vector<Tissue::Ptr>::iterator it = GetTissue(_tissueName);
  CheckTissue(it);

  // TODO(rhau) fwkHmNext instead of raw iterator
  vector<Cell::Coordinates> clone_locations;
  Tissue::CellIteratorConst cell_iter = (*it)->cellIterConst();
  for (; cell_iter != NULL; ++cell_iter) {
    clone_locations.push_back((*cell_iter)->location());
  }

  int i;
  for (; i<clone_locations.size(); i++) {
    Cell::Coordinates coord = clone_locations[i];
    try {
      CloneCell(_tissueName, coord, _side);        
    } catch (...) {
      cout << "caught clone cell exception" << endl;
    }
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
    throw "Tissue did not exist exception";
    return;
  }
}

Cell::Coordinates Simulation::GetCellLocation(Cell::Coordinates _loc, CellMembrane::Side _side) {
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
    throw "Location not found exception";
  }

  return clone_loc;
}

CellMembrane::Side Simulation::oppositeSide(CellMembrane::Side side) {
  if (side == CellMembrane::north_) {
    return CellMembrane::south_;
  }
  if (side == CellMembrane::south_) {
    return CellMembrane::north_;
  }
  if (side == CellMembrane::east_) {
    return CellMembrane::west_;
  }
  if (side == CellMembrane::west_) {
    return CellMembrane::east_;
  }

  throw "side not found exception";
}
