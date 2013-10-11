#include <queue>
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

  Stats::Ptr stats = Stats::StatsNew(newTissue);
  statuses_.push_back(stats);
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
                                AntibodyStrength _strength, Stats::Ptr _stats) {
  if (_cell->health() == Cell::infected_) return false;

  int strengthDiff = _strength.value() - _cell->membrane(_side)->antibodyStrength().value();
  _stats->onInfectionAttempt(_cell, strengthDiff); 
  if (strengthDiff <= 0) return false;

  _cell->healthIs(Cell::infected_);
  _stats->onCellInfected(_cell);
  return true;
}


void Simulation::infectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                             CellMembrane::Side _side, AntibodyStrength _strength) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::Ptr cell = tissue->cell(_loc);
  if (!cell.ptr()) return;

  Stats::Ptr stats = Simulation::stats(_tissueName);
  stats->attemptsIs(0);
  stats->strengthDiffIs(0);
  stats->pathLengthIs(0);

  std::queue<Cell::Ptr> fringe;
  std::queue<int> pathLengths;

  if (infectedCellIs(cell, _side, _strength, stats)) {
    fringe.push(cell);
    pathLengths.push(1);
    stats->pathLengthIs(1);
  }

  while (!fringe.empty()) {
    cell = fringe.front(); 
    int pathLength = pathLengths.front();
    stats->pathLengthIs(pathLength);

    for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
      CellMembrane::Side side = (CellMembrane::Side) i;
      Cell::Ptr target = tissue->cell(translatedLoc(cell->location(), side));
      if (target.ptr() && infectedCellIs(target, reversedSide(side), _strength, stats)) {
        fringe.push(target);
        pathLengths.push(pathLength + 1);
      }
    }
      
    fringe.pop();
    pathLengths.pop();
  }

  cout << stats->output() << endl;
}

void Simulation::printStats(Stats::Ptr stats) {
  
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

void Simulation::clonedCellIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                           CellMembrane::Side _side) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;
  Cell::PtrConst cell = tissue->cell(_loc);
  if (!cell.ptr()) return;

  Cell::Coordinates newLoc = translatedLoc(_loc, _side);
  cellIs(_tissueName, cell->cellType(), newLoc);

  // Copy over antibody strengths
  Cell::Ptr newCell = tissue->cell(newLoc);
  for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
    CellMembrane::Side side = (CellMembrane::Side) i;
    newCell->membrane(side)->antibodyStrengthIs(
      cell->membrane(side)->antibodyStrength());
  }
}

void Simulation::clonedCellsAre(Fwk::String _tissueName, CellMembrane::Side _side) {
  Tissue::Ptr tissue = Simulation::tissue(_tissueName);
  if (!tissue.ptr()) return;

  vector<Cell::Coordinates> cloneLocs;
  for (Tissue::CellIteratorConst it = tissue->cellIterConst(); it; ++it) {
    cloneLocs.push_back((*it)->location());
  }

  for (U32 i = 0; i < cloneLocs.size(); ++i) {
    try {
      clonedCellIs(_tissueName, cloneLocs[i], _side);
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


/*********/
/* Stats */
/*********/

void Simulation::Stats::onCellNew(Cell::Ptr _cell) {
  AntibodyStrength strength;
  switch (_cell->cellType()) {
    case Cell::cytotoxicCell_ :
      cytotoxicCount_++;
      strength.valueIs(100);
      break;
    case Cell::helperCell_ :
      helperCount_++;
      strength.valueIs(0);
      break;
    default : throw std::runtime_error("Invalid cell type");
  }

  // Break abstraction to manually iterate through sides
  for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
    CellMembrane::Side side = (CellMembrane::Side) i;
    CellMembrane::Ptr membrane = _cell->membraneNew(MEMBRANE_NAME, side);
    membrane->antibodyStrengthIs(strength); 
  }
}

void Simulation::Stats::onCellDel(Cell::Ptr _cell) {
  if (_cell->health() == Cell::infected_) infected_ -= 1;
  switch (_cell->cellType()) {
    case Cell::cytotoxicCell_ :
      cytotoxicCount_--;
      break;
    case Cell::helperCell_:
      helperCount_--;
      break;
    default : throw std::runtime_error("Invalid cell type");
  }
}

void Simulation::Stats::onCellInfected(Cell::Ptr _cell) {
  infected_ += 1;
  boundingBox_.limitIs(_cell);
}

void Simulation::Stats::onInfectionAttempt(Cell::Ptr _cell, int _strengthDiff) {
  attempts_ += 1;
  strengthDiff_ += _strengthDiff;
}

Fwk::String Simulation::Stats::output() {
  std::stringstream ss;
  ss << infected_ << " ";
  ss << attempts_ << " ";
  ss << strengthDiff_ << " ";
  ss << cytotoxicCount_ << " ";
  ss << helperCount_ << " ";
  ss << boundingBox_.spread()  << " ";
  ss << pathLength_ << " ";
  return ss.str();
}

void Simulation::Stats::BoundingBox::limitIs(Cell::Ptr _cell) {
  Cell::Coordinates loc = _cell->location(); 
  if (loc.x + 1 > max_.x) max_.x = loc.x + 1;
  if (loc.y + 1 > max_.y) max_.y = loc.y + 1;
  if (loc.y + 1 > max_.z) max_.z = loc.z + 1;
  if (loc.y + 1 <  min_.x) min_.x = loc.x;
  if (loc.y + 1 <  min_.y) min_.y = loc.y;
  if (loc.y + 1 <  min_.z) min_.z = loc.z;
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

Simulation::Stats::Ptr Simulation::stats(Fwk::String _tissueName) {
  Stats::Ptr stats;
  for (StatsList::iterator it = statuses_.begin(); it != statuses_.end(); ++it) {
    stats = *it;
    if (!strcmp(stats->name().c_str(), _tissueName.c_str()))
      break;
  }
  return stats;
}

Cell::Coordinates Simulation::translatedLoc(Cell::Coordinates _loc, CellMembrane::Side _side) {
  Cell::Coordinates newLoc = _loc;
  switch (_side) {
    case CellMembrane::north_ : newLoc.y += 1; break;
    case CellMembrane::south_ : newLoc.y -= 1; break;
    case CellMembrane::east_  : newLoc.x += 1; break;
    case CellMembrane::west_  : newLoc.x -= 1; break;
    case CellMembrane::up_    : newLoc.z += 1; break;
    case CellMembrane::down_  : newLoc.z -= 1; break;
    default : throw std::runtime_error("Invalid side");
  }
  return newLoc;
}

CellMembrane::Side Simulation::reversedSide(CellMembrane::Side _side) {
  switch (_side) {
    case CellMembrane::north_ : return CellMembrane::south_;
    case CellMembrane::south_ : return CellMembrane::north_;
    case CellMembrane::east_  : return CellMembrane::west_;
    case CellMembrane::west_  : return CellMembrane::east_;
    case CellMembrane::up_    : return CellMembrane::down_;
    case CellMembrane::down_  : return CellMembrane::up_;
    default : throw std::runtime_error("Invalid side");
  }
}
