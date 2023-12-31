#ifndef RecoMuon_TrackerSeedGenerator_TSGFromOrderedHits_H
#define RecoMuon_TrackerSeedGenerator_TSGFromOrderedHits_H

#include "RecoMuon/TrackerSeedGenerator/interface/TrackerSeedGenerator.h"
#include "FWCore/Framework/interface/FrameworkfwdMostUsed.h"
#include "DataFormats/Provenance/interface/RunID.h"

class SeedGeneratorFromRegionHits;
class TrackingRegion;

class TSGFromOrderedHits : public TrackerSeedGenerator {
public:
  TSGFromOrderedHits(const edm::ParameterSet &pset, edm::ConsumesCollector &iC);

  ~TSGFromOrderedHits() override;

private:
  void run(TrajectorySeedCollection &seeds,
           const edm::Event &ev,
           const edm::EventSetup &es,
           const TrackingRegion &region) override;

  edm::RunNumber_t theLastRun;
  std::unique_ptr<SeedGeneratorFromRegionHits> theGenerator;
};

#endif
