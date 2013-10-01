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

  const void AddTissue(Fwk::String _name);
  const void AddCell(Cell::Coordinates _loc, Fwk::String tissue_name,
      Cell::CellType _type);

 private:
  vector< Fwk::Ptr<Tissue> > tissues_;
};

#endif