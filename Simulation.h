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

  void TissueIs (const Fwk::String _name);
  void CellIs (Cell::Coordinates _loc, Fwk::String tissue_name,
      Cell::CellType _type);

  void InfectionIs(Cell::Coordinates _loc, CellMembrane::Side _side,
                   int _strength);

  void InfectedCellsDeleted(Fwk::String tissue_name);

 protected:
  vector< Fwk::Ptr<Tissue> > tissues_;

 private:
  std::vector<Fwk::Ptr<Tissue> >::iterator GetTissue(const Fwk::String _name);
};

#endif