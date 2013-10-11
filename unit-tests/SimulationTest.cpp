#include "gtest/gtest.h"
#include "../Simulation.h"

#define TISSUE "Tissue1"
#define MULTI "Tissue2"
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
    sim.tissueIs(TISSUE);
    sim.cellIs(TISSUE, Cell::cytotoxicCell_, Loc(0,0,0));

    sim.tissueIs(MULTI);
    sim.cellIs(MULTI, Cell::cytotoxicCell_, Loc(0,0,0));
    sim.cellIs(MULTI, Cell::cytotoxicCell_, Loc(1,0,0));
    sim.cellIs(MULTI, Cell::helperCell_, Loc(0,1,0));
  }

  Simulation sim;
};

/*********/
/* Tests */
/*********/

TEST_F(SimulationTest, TissueIs) {
  Tissue::Ptr tissue = sim.tissue(TISSUE);
  ASSERT_TRUE(tissue.ptr());
  ASSERT_EQ(strcmp(tissue->name().c_str(), TISSUE), 0);
}

TEST_F(SimulationTest, CellIs) {
  Cell::Ptr cell = sim.tissue(TISSUE)->cell(Loc(0,0,0));
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
  sim.antibodyStrengthIs(TISSUE, Loc(0,0,0), CellMembrane::north_, strength);
  Cell::Ptr cell = sim.tissue(TISSUE)->cell(Loc(0,0,0));
  ASSERT_EQ(cell->membrane(CellMembrane::north_)->antibodyStrength(), strength);
}

TEST_F(SimulationTest, ClonedCellIs) {
  // Tweak antibody strength to make sure it's copied
  Tissue::Ptr tissue = sim.tissue(TISSUE);
  AntibodyStrength strength(50);
  sim.antibodyStrengthIs(TISSUE, Loc(0,0,0), CellMembrane::north_, strength);

  sim.clonedCellIs(TISSUE, Loc(0,0,0), CellMembrane::south_);
  Cell::Ptr clonedCell = tissue->cell(Loc(0,-1,0));
  ASSERT_TRUE(clonedCell.ptr());
  ASSERT_EQ(clonedCell->membrane(CellMembrane::north_)->antibodyStrength(), strength);

  // Should throw exception on existing cell
  tissue = sim.tissue(MULTI);
  try {
    sim.clonedCellIs(MULTI, Loc(0,0,0), CellMembrane::north_);
    ASSERT_TRUE(false);
  } catch (...) {
    // Existing cell should not be overwritten
    ASSERT_EQ(tissue->cell(Loc(0,1,0))->cellType(), Cell::helperCell_); 
  }
}

TEST_F(SimulationTest, ClonedCellsAre) {
  Tissue::Ptr tissue = sim.tissue(MULTI);   
  sim.clonedCellsAre(MULTI, CellMembrane::north_);
  ASSERT_TRUE(tissue->cell(Loc(1,1,0)).ptr());
  ASSERT_EQ(tissue->cell(Loc(0,1,0))->cellType(), Cell::helperCell_); 
}

TEST_F(SimulationTest, InfectionDel) {
  Tissue::Ptr tissue = sim.tissue(MULTI);  
}

TEST_F(SimulationTest, CellCounts) {
  Stats::Ptr stats = sim.stats(MULTI);
  ASSERT_EQ(stats->infected(), 0);
  ASSERT_EQ(stats->cytotoxicCount(), 2);
  ASSERT_EQ(stats->helperCount(), 1);

  /* Test counts after clone */
  sim.clonedCellsAre(MULTI, CellMembrane::east_);
  ASSERT_EQ(stats->cytotoxicCount(), 3);
  ASSERT_EQ(stats->helperCount(), 2);
}

