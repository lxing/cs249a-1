//#include <limits.h>
#include "Simulation.h"
//#include "fwk/Types.h"
#include "gtest/gtest.h"

#define TestTissue "Tissue1"

class SimulationTest : public Simulation {
public:
  SimulationTester() : Simulation() {}

  vector<Tissue::Ptr>* GetTissues() {
    return &tissues_;
  }
};

Cell::Coordinates coords(int x, int y, int z) {
  Cell::Coordinates loc;
  loc.x = 0;
  loc.y = 0;
  loc.z = 0;
  return loc;
}

void TestSetup (SimulationTester& simulation) {
  simulation.TissueIs(kTestTissueName);

  vector<Tissue::Ptr>* tissues = simulation.GetTissues();
  assert(tissues->size() == 1);
  assert(strcmp(tissues->at(0)->name().c_str(), kTestTissueName)==0);

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  simulation.CellIs(kTestTissueName, Cell::tCell_, coord);
}

// Tests factorial of negative numbers.
TEST(SimulationTest, Simple) {
  SimulationTester simulation;
  SimulationTestSetup(simulation);
  vector<Tissue::Ptr>* tissues = simulation.GetTissues();

  printf("%d\n", tissues->at(0)->cells());
  // Tests to see if a cell was indeed created
  assert(tissues->at(0)->cells()==U32(1));
}

TEST(SimulationTest, Delete) {
  SimulationTester simulation;
  SimulationTestSetup(simulation);
  vector<Tissue::Ptr>* tissues = simulation.GetTissues();

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  Cell::Ptr cell = tissues->at(0)->cell(coord);
  cell->healthIs(Cell::infected_);
  simulation.InfectedCellsDel(kTestTissueName);
  // Tests to see if the infected cell was deleted
  assert(tissues->at(0)->cells()==U32(0));
}

TEST(SimulationTest, Clone) {
  SimulationTester simulation;
  SimulationTestSetup(simulation);
  vector<Tissue::Ptr>* tissues = simulation.GetTissues();
  assert(tissues->at(0)->cells() == U32(1));

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  simulation.CloneCell(kTestTissueName, coord, CellMembrane::east_);
  // Tests to see that a cell was cloned
  assert(tissues->at(0)->cells() == U32(2));
}

TEST(SimulationTest, CloneMultiple) {
  SimulationTester simulation;
  SimulationTestSetup(simulation);
  vector<Tissue::Ptr>* tissues = simulation.GetTissues();

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  simulation.CloneCell(kTestTissueName, coord, CellMembrane::east_);
  assert(tissues->at(0)->cells() == U32(2));

  // Tests to see that two cells were cloned
  simulation.CloneCells(kTestTissueName, CellMembrane::north_);
  assert(tissues->at(0)->cells() == U32(4));
}

TEST(SimulationTest, Infection) {
  SimulationTester simulation;
  SimulationTestSetup(simulation);
  vector<Tissue::Ptr>* tissues = simulation.GetTissues();

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  AntibodyStrength strength(100);
  simulation.InfectionIs(kTestTissueName, coord, CellMembrane::north_, strength);

  Cell::Ptr cell = tissues->at(0)->cell(coord);
  assert(Cell::infected_ == cell->health());
}

TEST(SimulationTest, AntibodyStrength) {
  SimulationTester simulation;
  SimulationTestSetup(simulation);
  vector<Tissue::Ptr>* tissues = simulation.GetTissues();

  Cell::Coordinates coord;
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  Cell::Ptr cell = tissues->at(0)->cell(coord);

  simulation.AntibodyStrengthIs(kTestTissueName, coord, CellMembrane::north_, 0);
  AntibodyStrength expected_strength(0);
  assert(expected_strength ==
      cell->membrane(CellMembrane::north_)->antibodyStrength());
}
