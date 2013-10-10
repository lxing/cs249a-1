#ifndef SIMULATION_H
#define SIMULATION_H

#include <cmath>
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

    static TissueReactor* TissueReactorIs(Tissue* t, Simulation* s) {
      TissueReactor* tr = new TissueReactor(t, s);
      return tr;
    }

   protected:
    TissueReactor(Tissue* t, Simulation* s) : Tissue::Notifiee(), owner_(s) {
      notifierIs(t);
    }

   private:
    Simulation* owner_;
  };

  class SimulationStats {
   public:
    SimulationStats();
    ~SimulationStats() {};
    void incNumInfectedCells() { ++numInfectedCells_; }
    void incNumInfectionAttempts() { ++numInfectionAttempts_; }
    void incTotalDiseaseAndAntibodyStrengthDiff() { 
      ++totalDiseaseAndAntibodyStrengthDiff_;
    }
    void incNumLiveCytotoxicCells() { ++numLiveCytotoxicCells_; }
    void incNumLiveHelperCells() { ++numLiveHelperCells_; }
    void incLongestInfectionPathLength() { ++longestInfectionPathLength_; }

    void RootLocIs(Cell::Coordinates _root_loc) {
      root_loc_ = _root_loc;
      north_loc_ = _root_loc;
      south_loc_ = _root_loc;
      east_loc_ = _root_loc;
      west_loc_ = _root_loc;
      top_loc_ = _root_loc;
      bottom_loc_ = _root_loc;
    }

    void UpdateSpread(Cell::Coordinates _loc);
    void UpdatePathLength(Cell::Coordinates _loc);
    string ToString();

   private:
    void CalculateInfectionSpread();

    U64 numInfectedCells_;
    U32 numInfectionAttempts_;
    U64 totalDiseaseAndAntibodyStrengthDiff_;
    U64 numLiveCytotoxicCells_;
    U64 numLiveHelperCells_;
    U64 infectionSpread_;
    U64 longestInfectionPathLength_;

    Cell::Coordinates root_loc_;
    Cell::Coordinates north_loc_;
    Cell::Coordinates south_loc_;
    Cell::Coordinates east_loc_;
    Cell::Coordinates west_loc_;
    Cell::Coordinates top_loc_;
    Cell::Coordinates bottom_loc_;
  };

  class PathTracker{
    
    Cell::Ptr next_cell;
    U32 path_length;
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

  void incNumLiveCytotoxicCells() { stats_.incNumLiveCytotoxicCells(); }
  void incNumLiveHelperCells() { stats_.incNumLiveHelperCells(); }


 protected:
  vector<Tissue::Ptr> tissues_;
  vector<Tissue*> tissue_ptrs_;

 private:
  bool InfectedCellIs(Cell::Ptr _cell, CellMembrane::Side _side, 
      AntibodyStrength _strength);
  std::vector<Tissue::Ptr>::iterator GetTissue (const Fwk::String _name);
  void CheckTissue (const std::vector<Tissue::Ptr>::iterator it);
  Cell::Coordinates GetCellLocation(Cell::Coordinates _loc, CellMembrane::Side _side);

  SimulationStats stats_;
};

#endif
