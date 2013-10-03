
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

  vector< Fwk::Ptr<Tissue> >* GetTissues() {
    return &tissues_;
  }
};


// Tests factorial of negative numbers.
TEST(SimulationTest, Simple) {
  SimulationTester simulation;
  simulation.TissueIs("TestTissue");

  vector< Fwk::Ptr<Tissue> >* tissues = simulation.GetTissues();
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

// Step 3. Call RUN_ALL_TESTS() in main().
//
// We do this by linking in src/gtest_main.cc file, which consists of
// a main() function which calls RUN_ALL_TESTS() for us.
//
// This runs all the tests you've defined, prints the result, and
// returns 0 if successful, or 1 otherwise.
//
// Did you notice that we didn't register the tests?  The
// RUN_ALL_TESTS() macro magically knows about all the tests we
// defined.  Isn't this convenient?

