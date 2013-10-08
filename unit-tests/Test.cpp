#include "gtest/gtest.h"
#include "../Simulation.h"


#define TISSUE_NAME "Tissue1"
#define SIDE_MIN 0
#define SIDE_MAX 6

/******************/
/* Initialization */
/******************/

typedef std::vector<Tissue::Ptr> TissueList; 

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

TEST_F(SimulationTest, CloneCell) {
  Tissue::Ptr tissue = multiSim.tissue(TISSUE_NAME);
  multiSim.cloneCell(TISSUE_NAME, Loc(0,0,0), CellMembrane::south_);
  Cell::Ptr clonedCell = tissue->cell(Loc(0,-1,0));
  //ASSERT_TRUE(clonedCell.ptr());
}

TEST_F(SimulationTest, CloneCells) {
}
