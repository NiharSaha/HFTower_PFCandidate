#ifndef RecoEGAMMA_ConversionTrack_InOutConversionTrackFinder_h
#define RecoEGAMMA_ConversionTrack_InOutConversionTrackFinder_h
/** \class InOutConversionTrackFinder
 **  
 **
 **  \author Nancy Marinelli, U. of Notre Dame, US
 **
 ***/

//
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidateCollection.h"
//
#include "RecoTracker/CkfPattern/interface/RedundantSeedCleaner.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
#include "RecoEgamma/EgammaPhotonAlgos/interface/ConversionTrackFinder.h"

// C/C++ headers
#include <string>
#include <vector>

//

class MagneticField;
class TrajectoryCleanerBySharedHits;

class InOutConversionTrackFinder : public ConversionTrackFinder {
public:
  InOutConversionTrackFinder(const edm::ParameterSet& config,
                             const BaseCkfTrajectoryBuilder* trajectoryBuilder,
                             edm::ConsumesCollector iC);

  ~InOutConversionTrackFinder() override;
  std::vector<Trajectory> tracks(const TrajectorySeedCollection& seeds,
                                 TrackCandidateCollection& candidate) const override;

private:
  TrajectoryCleanerBySharedHits* theTrajectoryCleaner_;
  RedundantSeedCleaner* theSeedCleaner_;
};

#endif
