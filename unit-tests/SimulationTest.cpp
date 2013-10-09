#include "gtest/gtest.h"
#include "../Simulation.h"

#define TISSUE_NAME "Tissue1"
#define SIDE_MIN 0
#define SIDE_MAX 6

/******************/
/* Initialization */
/******************/

typedef Simulation::Stats Stats;

Cell::Coordinates Loc(int x, int y, int z) {
  Cell::Coordinates loc;
  loc.x = x;
  loc.y = y;
  loc.z = z;
  return loc;
}

class SimulationTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    simpleSim.tissueIs(TISSUE_NAME);
    simpleSim.cellIs(TISSUE_NAME, Cell::cytotoxicCell_, Loc(0,0,0));

    multiSim.tissueIs(TISSUE_NAME);
    multiSim.cellIs(TISSUE_NAME, Cell::cytotoxicCell_, Loc(0,0,0));
    multiSim.cellIs(TISSUE_NAME, Cell::cytotoxicCell_, Loc(1,0,0));
    multiSim.cellIs(TISSUE_NAME, Cell::helperCell_, Loc(0,1,0));
  }

  Simulation simpleSim;
  Simulation multiSim;
};

/*********/
/* Tests */
/*********/

TEST_F(SimulationTest, TissueIs) {
  Tissue::Ptr tissue = simpleSim.tissue(TISSUE_NAME);
  ASSERT_TRUE(tissue.ptr());
  ASSERT_EQ(strcmp(tissue->name().c_str(), TISSUE_NAME), 0);
}

TEST_F(SimulationTest, CellIs) {
  Cell::Ptr cell = simpleSim.tissue(TISSUE_NAME)->cell(Loc(0,0,0));
  ASSERT_TRUE(cell.ptr());
  ASSERT_EQ(cell->cellType(), Cell::cytotoxicCell_);

  AntibodyStrength strength(100);
  for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
    CellMembrane::Side side = (CellMembrane::Side) i;
    CellMembrane::Ptr membrane = cell->membrane(side);
    ASSERT_TRUE(membrane.ptr());
    ASSERT_EQ(membrane->antibodyStrength(), strength);
  }
}

TEST_F(SimulationTest, AntibodyStrengthIs) { 
  AntibodyStrength strength(50);
  simpleSim.antibodyStrengthIs(TISSUE_NAME, Loc(0,0,0), CellMembrane::north_, strength);
  Cell::Ptr cell = simpleSim.tissue(TISSUE_NAME)->cell(Loc(0,0,0));
  ASSERT_EQ(cell->membrane(CellMembrane::north_)->antibodyStrength(), strength);
}

TEST_F(SimulationTest, ClonedCellIs) {
  // Tweak an antibody strength to make sure it's copied
  Tissue::Ptr tissue = simpleSim.tissue(TISSUE_NAME);
  AntibodyStrength strength(50);
  simpleSim.antibodyStrengthIs(TISSUE_NAME, Loc(0,0,0), CellMembrane::north_, strength);

  simpleSim.clonedCellIs(TISSUE_NAME, Loc(0,0,0), CellMembrane::south_);
  Cell::Ptr clonedCell = tissue->cell(Loc(0,-1,0));
  ASSERT_TRUE(clonedCell.ptr());
  ASSERT_EQ(clonedCell->membrane(CellMembrane::north_)->antibodyStrength(), strength);

  // Should throw exception on existing cell
  tissue = multiSim.tissue(TISSUE_NAME);
  try {
    multiSim.clonedCellIs(TISSUE_NAME, Loc(0,0,0), CellMembrane::north_);
    ASSERT_TRUE(false);
  } catch (...) {
    // Existing cell should not be overwritten
    ASSERT_EQ(tissue->cell(Loc(0,1,0))->cellType(), Cell::helperCell_); 
  }
}

TEST_F(SimulationTest, ClonedCellsAre) {
  Tissue::Ptr tissue = multiSim.tissue(TISSUE_NAME);   
  multiSim.clonedCellsAre(TISSUE_NAME, CellMembrane::north_);
  ASSERT_TRUE(tissue->cell(Loc(1,1,0)).ptr());
  ASSERT_EQ(tissue->cell(Loc(0,1,0))->cellType(), Cell::helperCell_); 
}

TEST_F(SimulationTest, CellCounts) {
  Stats::Ptr stats = multiSim.stats(TISSUE_NAME);
  ASSERT_EQ(stats->infected(), 0);
  ASSERT_EQ(stats->cytotoxicCount(), 2);
  ASSERT_EQ(stats->helperCount(), 1);

  /* Test counts after clone */
  multiSim.clonedCellsAre(TISSUE_NAME, CellMembrane::east_);
  ASSERT_EQ(stats->cytotoxicCount(), 3);
  ASSERT_EQ(stats->helperCount(), 2);
}

TEST_F(SimulationTest, InfectionDel) {
}
