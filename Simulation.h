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
  void clonedCellIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                    CellMembrane::Side _side);
  void clonedCellsAre(Fwk::String _tissueName, CellMembrane::Side _side);
  void antibodyStrengthIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                          CellMembrane::Side _side, AntibodyStrength _strength);

  class Stats : public Tissue::Notifiee {
  public:
    typedef Fwk::Ptr<Stats> Ptr;
    void onCellNew(Cell::Ptr _cell);
    void onCellDel(Cell::Ptr _cell);
    void onCellInfected(Cell::Ptr _cell);
    void onInfectionAttempt(Cell::Ptr _cell, int _strengthDiff);
    Fwk::String name() { return name_; };

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
    void attemptsIs(int _attempts) { attempts_ = _attempts; };
    void strengthDiffIs(int _strengthDiff) { strengthDiff_ = _strengthDiff; };
    void pathLengthIs(int _pathLength) { pathLength_ = _pathLength; };
    Fwk::String output();

  protected:
    Stats(Tissue::Ptr _tissue): Tissue::Notifiee(),
        infected_(0),attempts_(0),strengthDiff_(0),cytotoxicCount_(0),
        helperCount_(0),spread_(0),pathLength_(0),name_(_tissue->name()) {
      notifierIs(_tissue);
    }

    int infected_;
    int attempts_;
    int strengthDiff_;
    int cytotoxicCount_;
    int helperCount_;
    int spread_;
    int pathLength_;
    Fwk::String name_;
  };

  typedef std::vector<Stats::Ptr> StatsList;

  Tissue::Ptr tissue(Fwk::String _tissueName);
  Stats::Ptr stats(Fwk::String _tissueName);

protected:
  TissueList tissues_;
  StatsList statuses_;

  bool infectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                      AntibodyStrength _strength, Stats::Ptr _stats);
  Cell::Coordinates translatedLoc(Cell::Coordinates _loc, CellMembrane::Side _side);
  CellMembrane::Side reversedSide(CellMembrane::Side _loc);
  void printStats(Stats::Ptr stats);
};

#endif
