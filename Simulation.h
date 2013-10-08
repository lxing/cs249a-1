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
    void onCellNew(Cell::Ptr _cell);
    static TissueReactor::Ptr TissueReactorIs(Tissue::Ptr _tissue) {
      return new TissueReactor(_tissue);
    }
  protected:
    TissueReactor(Tissue::Ptr _tissue) : Tissue::Notifiee() {
      notifierIs(_tissue);
    }
  };

protected:
  TissueList tissues_;
  bool InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
                      AntibodyStrength _strength);
  Tissue::Ptr GetTissue(Fwk::String _tissueName);
  Cell::Coordinates LocationMove(Cell::Coordinates _loc, CellMembrane::Side _side);
};

class SimulationCount {
public:
  U8 value() const { return value_; }
  void valueIs(U8 _value) { value_ = _value; }
  void valueInc(U8 _inc) { value_ += _inc; }
  SimulationCount(U8 _value=0): value_(_value) {
    if (_value < 0) throw Fwk::RangeException("value=range()");
  }
protected:
  U8 value_;
};

#endif
