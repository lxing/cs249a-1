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

  /* Event Reactor */
  class TissueReactor : public Tissue::Notifiee {
  public:
    void onCellNew(Cell::Ptr _cell);
    static TissueReactor::Ptr TissueReactorIs(Tissue::Ptr _tissue) {
      return new TissueReactor(_tissue);
    }
  protected:
    TissueReactor(Tissue::Ptr _tissue) : Tissue::Notifiee() {
      notifierIs(_tissue);
    }

    U32 infected_;
    U32 attempts_;
    U32 strengthDiff_;
    U32 cytotoxicCount_;
    U32 helperCount_;
    U32 spread_;
    U32 pathLength;
  };


protected:
  TissueList tissues_;
  //std::vector<Tissue::Ptr> reactors_;

  bool InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                      AntibodyStrength _strength);
  Tissue::Ptr GetTissue(Fwk::String _tissueName);
  Cell::Coordinates LocationMove(Cell::Coordinates _loc, CellMembrane::Side _side);
};

#endif
