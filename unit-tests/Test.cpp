#include "gtest/gtest.h"
#include "../Simulation.h"


#define TISSUE_NAME "Tissue1"
#define SIDE_MIN 0
#define SIDE_MAX 6

class SimulationTest : public ::testing::Test {
public:
  typedef std::vector<Tissue::Ptr> TissueList; 

  Cell::Coordinates Loc(int x, int y, int z) {
    Cell::Coordinates loc;
    loc.x = x;
    loc.y = y;
    loc.z = z;
    return loc;
  }

protected:
  virtual void SetUp() {
    simpleSim.TissueIs(TISSUE_NAME);
    simpleSim.CellIs(TISSUE_NAME, Cell::cytotoxicCell_, Loc(0,0,0));

    multiSim.TissueIs(TISSUE_NAME);
    multiSim.CellIs(TISSUE_NAME, Cell::cytotoxicCell_, Loc(0,0,0));
    multiSim.CellIs(TISSUE_NAME, Cell::cytotoxicCell_, Loc(1,0,0));
  }

  Simulation simpleSim;
  Simulation multiSim;
};


TEST_F(SimulationTest, TissueIs) {
  TissueList *tissues = simpleSim.Tissues(); 
  ASSERT_EQ(tissues->size(), 1);
  ASSERT_EQ(strcmp(tissues->at(0)->name().c_str(), TISSUE_NAME), 0);
}

TEST_F(SimulationTest, CellIs) {
  TissueList *tissues = simpleSim.Tissues(); 

  Cell::Ptr cell = tissues->at(0)->cell(SimulationTest::Loc(0,0,0));
  ASSERT_TRUE(cell.ptr());
  ASSERT_EQ(cell->cellType(), Cell::cytotoxicCell_);

  for (int i = SIDE_MIN; i < SIDE_MAX; i++) {
    CellMembrane::Side side = (CellMembrane::Side) i;
    CellMembrane::Ptr membrane = cell->membrane(side);
    ASSERT_TRUE(membrane.ptr());
  }
}

TEST_F(SimulationTest, antibodyStrengthIs) { 
  TissueList *tissues = simpleSim.Tissues(); 
  Cell::Ptr cell = tissues->at(0)->cell(SimulationTest::Loc(0,0,0));
}

TEST_F(SimulationTest, CloneCell) {
}

TEST_F(SimulationTest, CloneCells) {
}
