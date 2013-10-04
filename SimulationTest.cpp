
// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.

#include <limits.h>
#include "Simulation.h"
#include "fwk/Types.h"
#include "gtest/gtest.h"

class SimulationTester : public Simulation{
 public:
  SimulationTester() : Simulation() {}

  vector<Tissue::Ptr>* GetTissues() {
    return &tissues_;
  }
};


// Tests factorial of negative numbers.
TEST(SimulationTest, Simple) {
  SimulationTester simulation;
  simulation.TissueIs("TestTissue");

  vector<Tissue::Ptr>* tissues = simulation.GetTissues();
  assert(tissues->size() == 1);
  assert(strcmp(tissues->at(0)->name().c_str(), "TestTissue")==0);

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  simulation.CellIs("TestTissue", Cell::tCell_, coord);
  printf("%d\n", tissues->at(0)->cells());
  assert(tissues->at(0)->cells()==U32(1));
}

TEST(SimulationTest, Delete) {
  SimulationTester simulation;
  simulation.TissueIs("TestTissue");

  vector<Tissue::Ptr>* tissues = simulation.GetTissues();
  assert(tissues->size() == 1);
  assert(strcmp(tissues->at(0)->name().c_str(), "TestTissue")==0);

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  simulation.CellIs("TestTissue", Cell::tCell_, coord);

  Cell::Ptr cell = tissues->at(0)->cell(coord);
  cell->healthIs(Cell::infected_);
  simulation.InfectedCellsDel("TestTissue");
  assert(tissues->at(0)->cells()==U32(0));
}
