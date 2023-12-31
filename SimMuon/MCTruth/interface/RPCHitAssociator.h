#ifndef MCTruth_RPCHitAssociator_h
#define MCTruth_RPCHitAssociator_h

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "SimDataFormats/EncodedEventId/interface/EncodedEventId.h"
#include "SimDataFormats/RPCDigiSimLink/interface/RPCDigiSimLink.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"

class RPCHitAssociator {
public:
  typedef edm::DetSetVector<RPCDigiSimLink> RPCDigiSimLinks;
  typedef std::pair<uint32_t, EncodedEventId> SimHitIdpr;

  class Config {
  public:
    Config(const edm::ParameterSet &, edm::ConsumesCollector ic);

  private:
    friend class RPCHitAssociator;

    edm::InputTag RPCdigisimlinkTag;

    edm::InputTag RPCsimhitsTag;
    edm::InputTag RPCsimhitsXFTag;

    edm::EDGetTokenT<CrossingFrame<PSimHit>> RPCsimhitsXFToken_;
    edm::EDGetTokenT<edm::PSimHitContainer> RPCsimhitsToken_;
    edm::EDGetTokenT<edm::DetSetVector<RPCDigiSimLink>> RPCdigisimlinkToken_;

    bool crossingframe;
  };

  // Constructor with configurable parameters
  RPCHitAssociator(const edm::Event &e, const Config &conf);

  std::vector<SimHitIdpr> associateRecHit(const TrackingRecHit &hit) const;
  std::set<RPCDigiSimLink> findRPCDigiSimLink(uint32_t rpcDetId, int strip, int bx) const;
  //   const PSimHit* linkToSimHit(RPCDigiSimLink link);

private:
  void initEvent(const edm::Event &);

  Config const &theConfig;
  edm::Handle<edm::DetSetVector<RPCDigiSimLink>> _thelinkDigis;
  std::map<unsigned int, edm::PSimHitContainer> _SimHitMap;
};

#endif
