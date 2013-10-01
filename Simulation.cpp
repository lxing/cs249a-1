
#include "Simulation.h"

Simulation::Simulation() {
  // TODO(rhau) fill in
}

Simulation::~Simulation() {
  // TODO(rhau) fill in
}

const void Simulation::AddTissue(Fwk::String _name) {
  Fwk::Ptr<Tissue> ptr(new Tissue(_name));
}

const void Simulation::AddCell(Cell::Coordinates _loc, Fwk::String tissue_name,
                         Cell::CellType _type) {

}

