
// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.

#include <limits.h>
#include "Simulation.h"
#include "SimulationDispatcher.h"
#include "fwk/Types.h"
#include "gtest/gtest.h"

class FakeSimulation : public Simulation {
 public:
  FakeSimulation() {
    numTissueIs = 0;
    numCellIs = 0;
    numInfectionIs = 0;
    numInfectedCellsDel = 0;
    numCloneCell = 0;
    numCloneCells = 0;
    numAntiBodyStrengthIs = 0;
  }

  void TissueIs (const Fwk::String _name) {
    cout << "tissue is";
    ++numTissueIs;
  }
  void CellIs (Fwk::String _tissueName, Cell::CellType _type,
      Cell::Coordinates _loc) {
    ++numCellIs;
  }
  void InfectionIs (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side, AntibodyStrength _strength) {
    ++numInfectionIs;
  }
  void InfectedCellsDel (Fwk::String _tissueName) {
    ++numInfectedCellsDel;
  }
  void CloneCell (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side) {
    ++numCloneCell;
  }
  void CloneCells (Fwk::String _tissueName, CellMembrane::Side _side) {
    ++numCloneCells;
  }
  void AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side, AntibodyStrength _strength) {
    ++numAntiBodyStrengthIs;
  }

  int numTissueIs;
  int numCellIs;
  int numInfectionIs;
  int numInfectedCellsDel;
  int numCloneCell;
  int numCloneCells;
  int numAntiBodyStrengthIs;
};

TEST(SimulationDisptacherTest, TissueNewTest) {
  string line = "Tissue tissueNew Tissue1";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);
  assert (simulation.numTissueIs == 1);
}

TEST(SimulationDisptacherTest, CellNewTest) {
  string line = "Tissue Tissue1 helperCellNew 0 0 0";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);

  line = "Tissue Tissue1 cytotoxicCellNew 0 1 0";
  SimulationDispatcher::DispatchLine(line, &simulation);

  assert (simulation.numCellIs == 2);
}

TEST(SimulationDisptacherTest, CloneCellTest) {
  string line = "Cell Tissue1 0 1 0 cloneNew north";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);
  assert (simulation.numCloneCell == 1);
}

TEST(SimulationDisptacherTest, CloneCellsTest) {
  string line = "Tissue Tissue1 cloneCellsNew west";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);
  assert (simulation.numCloneCells == 1);
}

TEST(SimulationDisptacherTest, AntibodyStrengthIsTest) {
  string line = "Cell Tissue1 0 0 0 membrane south antibodyStrengthIs 100";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);
  assert (simulation.numAntiBodyStrengthIs == 1);
}

TEST(SimulationDisptacherTest, InfectionStartLocationIsTest) {
  string line = "Tissue Tissue1 infectionStartLocationIs 0 0 0 east 100";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);
  assert (simulation.numInfectionIs == 1);
}

TEST(SimulationDisptacherTest, InfectedCellsDelTest) {
  string line = "Tissue Tissue1 infectedCellsDel";
  FakeSimulation simulation;
  SimulationDispatcher::DispatchLine(line, &simulation);
  assert (simulation.numInfectedCellsDel == 1);
}