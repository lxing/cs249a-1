#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

#include "fwk/Ptr.h"
#include "fwk/String.h"
#include "Tissue.h"

using namespace std;

class Simulation {
 public:
  Simulation();
  virtual ~Simulation();

  class TissueReactor : public Tissue::Notifiee {
    void onCellNew(Cell::Ptr _cell);
  };

  void TissueIs (const Fwk::String _name);
  void CellIs (Fwk::String _tissueName, Cell::CellType _type,
      Cell::Coordinates _loc);
  void InfectionIs (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side, AntibodyStrength _strength);
  void InfectedCellsDel (Fwk::String _tissueName);

  void CloneCell (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side);
  void CloneCells (Fwk::String _tissueName, CellMembrane::Side _side);
  void AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side, AntibodyStrength _strength);


 protected:
  vector<Tissue::Ptr> tissues_;

 private:
  bool InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
      AntibodyStrength _strength);
  std::vector<Tissue::Ptr>::iterator GetTissue (const Fwk::String _name);
  void CheckTissue (const std::vector<Tissue::Ptr>::iterator it);
  Cell::Coordinates GetCloneLocation(Cell::Coordinates _loc, CellMembrane::Side _side);
};

class SimulationCount {
 public:
  U8 value() const { return value_; }
  void valueIs(U8 _value) { value_ = _value; }
  void valueInc(U8 _inc) { value_ += _inc; }
 protected:
  U8 value_;
};

#endif
