#ifndef SIMULATION_H
#define SIMULATION_H

#include <cmath>
#include <map>
#include <vector>

#include "fwk/Ptr.h"
#include "fwk/String.h"
#include "Tissue.h"

using namespace std;

class Simulation {
 public:
  Simulation();
  virtual ~Simulation();

  class SimulationStats : public Tissue::Notifiee {
   public:
    class InfectionSpread {
     public:
      enum SpreadState {
        empty_ = 0,
        not_empty_ = 1
      };

      InfectionSpread();
      ~InfectionSpread() {}
      
      void ResetSpread();
      void UpdateSpread(Cell::Coordinates _loc);
      U64 GetInfectionSpread();

     private:
      SpreadState state_;

      Cell::Coordinates root_loc_;
      Cell::Coordinates north_loc_;
      Cell::Coordinates south_loc_;
      Cell::Coordinates east_loc_;
      Cell::Coordinates west_loc_;
      Cell::Coordinates top_loc_;
      Cell::Coordinates bottom_loc_;
    };

    ~SimulationStats() {
      free(tissue_);
    }

    // Tissue::Notifiee Methods
    void onCellNew(Cell::Ptr _cell);
    static SimulationStats* SimulationStatsIs(Tissue* t) {
      SimulationStats* stats = new SimulationStats(t);
      return stats;
    }

    void onCellDel(Cell::Ptr _cell);

    void ResetInfectionStats();
    void ResetSpreadStats();
    void incNumInfectedCells() { ++numInfectedCells_; }
    void decNumInfectedCells() { --numInfectedCells_; }
    void incNumInfectionAttempts() { ++numInfectionAttempts_; }
    void incTotalDiseaseAndAntibodyStrengthDiff(int diff) { 
      totalDiseaseAndAntibodyStrengthDiff_ += diff;
    }
    void incNumLiveCytotoxicCells() { ++numLiveCytotoxicCells_; }
    void decNumLiveCytotoxicCells() { --numLiveCytotoxicCells_; }
    void incNumLiveHelperCells() { ++numLiveHelperCells_; }
    void decNumLiveHelperCells() { --numLiveHelperCells_; }
    void incLongestInfectionPathLength() { ++longestInfectionPathLength_; }

    // void RootLocIs(Cell::Coordinates _root_loc);

    void UpdateSpread(Cell::Coordinates _loc);
    // void UpdatePathLength(Cell::Coordinates _loc);
    string ToString();

   protected:
    SimulationStats(Tissue* t) : 
        Tissue::Notifiee(), tissue_(t),
        numInfectedCells_(0), numInfectionAttempts_(0),
        totalDiseaseAndAntibodyStrengthDiff_(0),
        numLiveCytotoxicCells_(0), numLiveHelperCells_(0),
        infectionSpread_(0), longestInfectionPathLength_(0) {
      notifierIs(t);
    }

   private:
    void CalculateInfectionSpread();

    Tissue* tissue_;
    InfectionSpread spread_;

    U64 numInfectedCells_;
    U32 numInfectionAttempts_;
    int totalDiseaseAndAntibodyStrengthDiff_;
    U64 numLiveCytotoxicCells_;
    U64 numLiveHelperCells_;
    U64 infectionSpread_;
    U64 longestInfectionPathLength_;
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

  // void incNumLiveCytotoxicCells() { stats_.incNumLiveCytotoxicCells(); }
  // void incNumLiveHelperCells() { stats_.incNumLiveHelperCells(); }


 protected:
  vector<Tissue::Ptr> tissues_;
  vector<Tissue*> tissue_ptrs_;

 private:
  bool InfectedCellIs(Fwk::String _tissueName, Cell::Ptr _cell, CellMembrane::Side _side, 
      AntibodyStrength _strength);
  std::vector<Tissue::Ptr>::iterator GetTissue (const Fwk::String _name);
  void CheckTissue (const std::vector<Tissue::Ptr>::iterator it);
  Cell::Coordinates GetCellLocation(Cell::Coordinates _loc, CellMembrane::Side _side);
  CellMembrane::Side oppositeSide(CellMembrane::Side side);

  std::map<string, SimulationStats*> stats_map_;
};

#endif
