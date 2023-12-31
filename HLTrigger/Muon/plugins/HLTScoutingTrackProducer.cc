// -*- C++ -*-
//
// Package:    HLTrigger/Muon
// Class:      HLTScoutingTrackProducer
//
/**\class HLTScoutingTrackProducer HLTScoutingTrackProducer.cc HLTScoutingTrackProducer.cc
Description: Producer for Run3 Scouting Tracks
*/
//

#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/getRef.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingTrack.h"
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"
#include "DataFormats/MuonReco/interface/MuonTrackLinks.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/Math/interface/libminifloat.h"

class HLTScoutingTrackProducer : public edm::global::EDProducer<> {
public:
  explicit HLTScoutingTrackProducer(const edm::ParameterSet&);
  ~HLTScoutingTrackProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID sid, edm::Event& iEvent, edm::EventSetup const& setup) const final;

  const edm::EDGetTokenT<reco::TrackCollection> otherTrackCollection_;
  const edm::EDGetTokenT<reco::VertexCollection> vertexCollection_;

  const int mantissaPrecision_;
  const double vtxMinDist_;
  const double ptMin_;
};

//
// constructors and destructor
//
HLTScoutingTrackProducer::HLTScoutingTrackProducer(const edm::ParameterSet& iConfig)
    : otherTrackCollection_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("OtherTracks"))),
      vertexCollection_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexCollection"))),
      mantissaPrecision_(iConfig.getParameter<int>("mantissaPrecision")),
      vtxMinDist_(iConfig.getParameter<double>("vtxMinDist")),
      ptMin_(iConfig.getParameter<double>("ptMin")) {
  //register products
  produces<Run3ScoutingTrackCollection>();
}

HLTScoutingTrackProducer::~HLTScoutingTrackProducer() = default;

// ------------ method called to produce the data  ------------
void HLTScoutingTrackProducer::produce(edm::StreamID sid, edm::Event& iEvent, edm::EventSetup const& setup) const {
  using namespace edm;

  std::unique_ptr<Run3ScoutingTrackCollection> outTrack(new Run3ScoutingTrackCollection());

  Handle<reco::TrackCollection> otherTrackCollection;
  Handle<reco::VertexCollection> vertexCollection;

  if (iEvent.getByToken(otherTrackCollection_, otherTrackCollection)) {
    //match tracks to vertices
    for (auto& trk : *otherTrackCollection) {
      int vtxInd = -1;
      double min_dist = vtxMinDist_;
      int vtxIt = 0;

      if (trk.pt() < ptMin_)
        continue;

      if (iEvent.getByToken(vertexCollection_, vertexCollection)) {
        for (auto& vrt : *vertexCollection) {
          double min_dist_tmp = pow(trk.dz(vrt.position()), 2);  // hltPixelVertices only clustered in Z

          if (min_dist_tmp < min_dist) {
            min_dist = min_dist_tmp;
            vtxInd = vtxIt;
          }

          vtxIt++;
        }
      }

      //fill track information
      outTrack->emplace_back(
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.pt(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.eta(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.phi(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.chi2(), mantissaPrecision_),
          trk.ndof(),
          trk.charge(),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.dxy(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.dz(), mantissaPrecision_),
          trk.hitPattern().numberOfValidPixelHits(),
          trk.hitPattern().trackerLayersWithMeasurement(),
          trk.hitPattern().numberOfValidStripHits(),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.qoverp(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.lambda(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.dxyError(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.dzError(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.qoverpError(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.lambdaError(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.phiError(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.dsz(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.dszError(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(0, 1), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(0, 2), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(0, 3), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(0, 4), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(1, 2), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(1, 3), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(1, 4), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(2, 3), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(2, 4), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.covariance(3, 4), mantissaPrecision_),
          vtxInd,
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.vx(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.vy(), mantissaPrecision_),
          MiniFloatConverter::reduceMantissaToNbitsRounding(trk.vz(), mantissaPrecision_));
    }
  }

  iEvent.put(std::move(outTrack));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HLTScoutingTrackProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("OtherTracks", edm::InputTag("hltPixelTracksL3MuonNoVtx"));
  desc.add<edm::InputTag>("vertexCollection", edm::InputTag("hltPixelVertices"));

  desc.add<int>("mantissaPrecision", 10)->setComment("default float16, change to 23 for float32");
  desc.add<double>("vtxMinDist", 0.01);
  desc.add<double>("ptMin", 0.3);
  descriptions.add("hltScoutingTrackProducer", desc);
}

// declare this class as a framework plugin
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HLTScoutingTrackProducer);
