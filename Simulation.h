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

  void TissueIs(Fwk::String _tissueName);
  void CellIs(Fwk::String _tissueName, Cell::CellType _type,
              Cell::Coordinates _loc);
  void InfectionIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                   CellMembrane::Side _side, AntibodyStrength _strength);
  void InfectionDel(Fwk::String _tissueName);
  void CloneCell(Fwk::String _tissueName, Cell::Coordinates _loc,
                 CellMembrane::Side _side);
  void CloneCells(Fwk::String _tissueName, CellMembrane::Side _side);
  void AntibodyStrengthIs(Fwk::String _tissueName, Cell::Coordinates _loc,
                          CellMembrane::Side _side, AntibodyStrength _strength);
  TissueList *Tissues() { return &tissues_; }

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


protected:
  TissueList tissues_;
  ReactorList reactors_;

  bool InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                      AntibodyStrength _strength);
  Tissue::Ptr GetTissue(Fwk::String _tissueName);
  TissueReactor::Ptr GetReactor(Fwk::String _tissueName);
  Cell::Coordinates LocationMove(Cell::Coordinates _loc, CellMembrane::Side _side);
};

#endif
