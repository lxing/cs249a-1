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

  class TissueReactor : public Tissue::Notifiee {
   public:
    void onCellNew(Cell::Ptr _cell);

    static TissueReactor* TissueReactorIs(Tissue* t) {
      TissueReactor* tr = new TissueReactor(t);
      return tr;
    }
   protected:
    TissueReactor(Tissue* t) : Tissue::Notifiee() {
      notifierIs(t);
    }
  };

  class SimulationStats {
   public:
    SimulationStats();
    ~SimulationStats() {};
    void incNumInfectedCells() { ++numInfectedCells; }
    void incNumInfectionAttempts() { ++numInfectionAttempts; }
    void incTotalDiseaseAndAntibodyStrengthDiff() { 
      ++totalDiseaseAndAntibodyStrengthDiff;
    }
    void incNumLiveCytotoxicCells() { ++numLiveCytotoxicCells; }
    void incNumLiveHelperCells() { ++numLiveHelperCells; }
    void incInfectionSpread() { ++infectionSpread; }
    void incLongestInfectionPathLength() { ++longestInfectionPathLength; }

    string ToString();

   private:
    U64 numInfectedCells;
    U32 numInfectionAttempts;
    U64 totalDiseaseAndAntibodyStrengthDiff;
    U64 numLiveCytotoxicCells;
    U64 numLiveHelperCells;
    U64 infectionSpread;
    U64 longestInfectionPathLength;
  };

  void TissueIs (const Fwk::String _name);
  void CellIs (Fwk::String _tissueName, Cell::CellType _type,
      Cell::Coordinates _loc);
  void InfectionIs (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side, AntibodyStrength _strength);
  void InfectedCellsDel (Fwk::String _tissueName);

  void CloneCell (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side);
  void CloneCells (Fwk::String _tissueName, CellMembrane::Side _side);
  void AntibodyStrengthIs (Fwk::String _tissueName, Cell::Coordinates _loc,
      CellMembrane::Side _side, AntibodyStrength _strength);


 protected:
  vector<Tissue::Ptr> tissues_;

 private:
  bool InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
      AntibodyStrength _strength);
  std::vector<Tissue::Ptr>::iterator GetTissue (const Fwk::String _name);
  void CheckTissue (const std::vector<Tissue::Ptr>::iterator it);
  Cell::Coordinates GetCloneLocation(Cell::Coordinates _loc, CellMembrane::Side _side);

  SimulationStats stats_;
};

#endif
