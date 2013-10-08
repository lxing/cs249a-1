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

  class TissueReactor : public Tissue::Notifiee {
  public:
    typedef Fwk::Ptr<TissueReactor> Ptr;
    void onCellNew(Cell::Ptr _cell);
    Tissue::Ptr tissue() { return tissue_; };

    static TissueReactor::Ptr TissueReactorNew(Tissue::Ptr _tissue) {
      TissueReactor::Ptr reactor = new TissueReactor(_tissue);
      reactor->referencesDec(1);
      return reactor;
    };

    U32 infected() { return infected_; };
    U32 attempts() { return attempts_; };
    U32 strengthDiff() { return strengthDiff_; };
    U32 cytotoxicCount() { return cytotoxicCount_; };
    U32 helperCount() { return helperCount_; };
    U32 spread() { return spread_; };
    U32 pathLength() { return pathLength_; };
    void infectedInc(U32 val) { infected_ += val; };
    void attemptsInc(U32 val) { attempts_ += val; };
    void cytotoxicCountInc(U32 val) { cytotoxicCount_ += val; };
    void helperCountInc(U32 val) { helperCount_ += val; };

  protected:
    TissueReactor(Tissue::Ptr _tissue) : Tissue::Notifiee() {
      tissue_ = _tissue;
      notifierIs(_tissue);
    }

    U32 infected_;
    U32 attempts_;
    U32 strengthDiff_;
    U32 cytotoxicCount_;
    U32 helperCount_;
    U32 spread_;
    U32 pathLength_;
    Tissue::Ptr tissue_;
  };

  typedef std::vector<TissueReactor::Ptr> ReactorList;

  Tissue::Ptr tissue(Fwk::String _tissueName);
  TissueReactor::Ptr reactor(Fwk::String _tissueName);

protected:
  TissueList tissues_;
  ReactorList reactors_;

  bool infectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                      AntibodyStrength _strength);
  Cell::Coordinates locationMove(Cell::Coordinates _loc, CellMembrane::Side _side);
};

#endif
