#ifndef SIMULATION_H
#define SIMULATION_H

#include "fwk/LinkedList.h"
#include "fwk/Ptr.h"
#include "fwk/String.h"
#include "Tissue.h"
#include <vector>

using namespace std;

class Simulation {
public:
  typedef std::vector<Tissue::Ptr> TissueList;

  Simulation();
  virtual ~Simulation();

  void tissueIs(Fwk::String _tissueName);
  void cellIs(Fwk::String _tissueName, Cell::CellType _type,
              Cell::Coordinates _loc);
  void infectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                   CellMembrane::Side _side, AntibodyStrength _strength);
  void infectionDel(Fwk::String _tissueName);
  void cloneCell(Fwk::String _tissueName, Cell::Coordinates _loc,
                 CellMembrane::Side _side);
  void cloneCells(Fwk::String _tissueName, CellMembrane::Side _side);
  void antibodyStrengthIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                          CellMembrane::Side _side, AntibodyStrength _strength);

  class Stats : public Tissue::Notifiee {
  public:
    typedef Fwk::Ptr<Stats> Ptr;
    void onCellNew(Cell::Ptr _cell);
    Tissue::Ptr tissue() { return tissue_; };

    static Stats::Ptr StatsNew(Tissue::Ptr _tissue) {
      Stats::Ptr stats = new Stats(_tissue);
      stats->referencesDec(1);
      return stats;
    };

    int infected() { return infected_; };
    int attempts() { return attempts_; };
    int strengthDiff() { return strengthDiff_; };
    int cytotoxicCount() { return cytotoxicCount_; };
    int helperCount() { return helperCount_; };
    int spread() { return spread_; };
    int pathLength() { return pathLength_; };
    void infectedInc(int val) { infected_ += val; };
    void attemptsInc(int val) { attempts_ += val; };
    void cytotoxicCountInc(int val) { cytotoxicCount_ += val; };
    void helperCountInc(int val) { helperCount_ += val; };

  protected:
    Stats(Tissue::Ptr _tissue) : Tissue::Notifiee() {
      tissue_ = _tissue;
      notifierIs(_tissue);
    }

    int infected_;
    int attempts_;
    int strengthDiff_;
    int cytotoxicCount_;
    int helperCount_;
    int spread_;
    int pathLength_;
    Tissue::Ptr tissue_;
  };

  typedef std::vector<Stats::Ptr> StatsList;

  Tissue::Ptr tissue(Fwk::String _tissueName);
  Stats::Ptr stats(Fwk::String _tissueName);

protected:
  TissueList tissues_;
  StatsList statuses_;

  bool infectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                      AntibodyStrength _strength);
  Cell::Coordinates locationMove(Cell::Coordinates _loc, CellMembrane::Side _side);
};

#endif
