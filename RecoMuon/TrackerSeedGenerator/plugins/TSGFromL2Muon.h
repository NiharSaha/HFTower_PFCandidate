#ifndef RecoMuon_TrackerSeedGenerator_TSGFromL2Muon_H
#define RecoMuon_TrackerSeedGenerator_TSGFromL2Muon_H

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonSeed/interface/L3MuonTrajectorySeed.h"
#include "DataFormats/MuonSeed/interface/L3MuonTrajectorySeedCollection.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include <vector>

// Forward declare classes from same package (RecoMuon):
class MuonServiceProxy;
class TrackerSeedGenerator;
class MuonTrackingRegionBuilder;
class TrackerSeedCleaner;
class TrackerTopologyRcd;

//
// Generate tracker seeds from L2 muons
//
class TSGFromL2Muon : public edm::stream::EDProducer<> {
public:
  TSGFromL2Muon(const edm::ParameterSet& cfg);
  ~TSGFromL2Muon() override;
  void beginRun(const edm::Run& run, const edm::EventSetup& es) override;
  void produce(edm::Event& ev, const edm::EventSetup& es) override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::InputTag theL2CollectionLabel;
  std::unique_ptr<MuonServiceProxy> theService;
  double thePtCut, thePCut;
  std::unique_ptr<MuonTrackingRegionBuilder> theRegionBuilder;
  std::unique_ptr<TrackerSeedGenerator> theTkSeedGenerator;
  std::unique_ptr<TrackerSeedCleaner> theSeedCleaner;
  edm::EDGetTokenT<reco::TrackCollection> l2muonToken;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> theTTopoToken;
};
#endif
