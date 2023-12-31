#ifndef HLTrigger_btau_L3MumuTrackingRegion_H
#define HLTrigger_btau_L3MumuTrackingRegion_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoTracker/Record/interface/TrackerMultipleScatteringRecord.h"
#include "RecoTracker/TkMSParametrization/interface/MultipleScatteringParametrisationMaker.h"
#include "RecoTracker/TkTrackingRegions/interface/TrackingRegionProducer.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

class L3MumuTrackingRegion : public TrackingRegionProducer {
public:
  L3MumuTrackingRegion(const edm::ParameterSet& cfg, edm::ConsumesCollector&& iC)
      : theFieldToken(iC.esConsumes()), theMSMakerToken(iC.esConsumes()) {
    edm::ParameterSet regionPSet = cfg.getParameter<edm::ParameterSet>("RegionPSet");

    theVertexTag = regionPSet.getParameter<edm::InputTag>("vertexSrc");
    theVertex = (theVertexTag.label().length() > 1);
    theInputTrkTag = regionPSet.getParameter<edm::InputTag>("TrkSrc");
    useVtxTks = regionPSet.getParameter<bool>("UseVtxTks");

    if (theVertex)
      theVertexToken = iC.consumes<reco::VertexCollection>(theVertexTag);
    if (!(theVertex && useVtxTks))
      theInputTrkToken = iC.consumes<reco::TrackCollection>(theInputTrkTag);

    thePtMin = regionPSet.getParameter<double>("ptMin");
    theOriginRadius = regionPSet.getParameter<double>("originRadius");
    theOriginHalfLength = regionPSet.getParameter<double>("originHalfLength");
    theOriginZPos = regionPSet.getParameter<double>("vertexZDefault");

    theDeltaEta = regionPSet.getParameter<double>("deltaEtaRegion");
    theDeltaPhi = regionPSet.getParameter<double>("deltaPhiRegion");
    if (regionPSet.exists("searchOpt")) {
      m_searchOpt = regionPSet.getParameter<bool>("searchOpt");
    } else {
      m_searchOpt = false;
    }
    m_howToUseMeasurementTracker = RectangularEtaPhiTrackingRegion::stringToUseMeasurementTracker(
        regionPSet.getParameter<std::string>("howToUseMeasurementTracker"));
    if (m_howToUseMeasurementTracker != RectangularEtaPhiTrackingRegion::UseMeasurementTracker::kNever) {
      theMeasurementTrackerToken =
          iC.consumes<MeasurementTrackerEvent>(regionPSet.getParameter<edm::InputTag>("measurementTrackerName"));
    }
  }

  ~L3MumuTrackingRegion() override = default;

  std::vector<std::unique_ptr<TrackingRegion> > regions(const edm::Event& ev,
                                                        const edm::EventSetup& es) const override {
    std::vector<std::unique_ptr<TrackingRegion> > result;

    const MeasurementTrackerEvent* measurementTracker = nullptr;
    if (!theMeasurementTrackerToken.isUninitialized()) {
      edm::Handle<MeasurementTrackerEvent> hmte;
      ev.getByToken(theMeasurementTrackerToken, hmte);
      measurementTracker = hmte.product();
    }
    const auto& field = es.getData(theFieldToken);
    const auto& msmaker = es.getData(theMSMakerToken);

    // optional constraint for vertex
    // get highest Pt pixel vertex (if existing)
    double deltaZVertex = theOriginHalfLength;
    double originz = theOriginZPos;
    if (theVertex) {
      edm::Handle<reco::VertexCollection> vertices;
      ev.getByToken(theVertexToken, vertices);
      const reco::VertexCollection vertCollection = *(vertices.product());
      reco::VertexCollection::const_iterator ci = vertCollection.begin();
      if (!vertCollection.empty()) {
        originz = ci->z();
      } else {
        originz = theOriginZPos;
        deltaZVertex = 15.;
      }
      if (useVtxTks) {
        for (ci = vertCollection.begin(); ci != vertCollection.end(); ci++)
          for (reco::Vertex::trackRef_iterator trackIt = ci->tracks_begin(); trackIt != ci->tracks_end(); trackIt++) {
            reco::TrackRef iTrk = (*trackIt).castTo<reco::TrackRef>();
            GlobalVector dirVector((iTrk)->px(), (iTrk)->py(), (iTrk)->pz());
            result.push_back(std::make_unique<RectangularEtaPhiTrackingRegion>(dirVector,
                                                                               GlobalPoint(0, 0, float(ci->z())),
                                                                               thePtMin,
                                                                               theOriginRadius,
                                                                               deltaZVertex,
                                                                               theDeltaEta,
                                                                               theDeltaPhi,
                                                                               field,
                                                                               &msmaker,
                                                                               true,
                                                                               m_howToUseMeasurementTracker,
                                                                               measurementTracker,
                                                                               m_searchOpt));
          }
        return result;
      }
    }

    edm::Handle<reco::TrackCollection> trks;
    if (!theInputTrkToken.isUninitialized())
      ev.getByToken(theInputTrkToken, trks);
    for (reco::TrackCollection::const_iterator iTrk = trks->begin(); iTrk != trks->end(); iTrk++) {
      GlobalVector dirVector((iTrk)->px(), (iTrk)->py(), (iTrk)->pz());
      result.push_back(std::make_unique<RectangularEtaPhiTrackingRegion>(dirVector,
                                                                         GlobalPoint(0, 0, float(originz)),
                                                                         thePtMin,
                                                                         theOriginRadius,
                                                                         deltaZVertex,
                                                                         theDeltaEta,
                                                                         theDeltaPhi,
                                                                         field,
                                                                         &msmaker,
                                                                         true,
                                                                         m_howToUseMeasurementTracker,
                                                                         measurementTracker,
                                                                         m_searchOpt));
    }
    return result;
  }

private:
  edm::InputTag theVertexTag;
  bool theVertex;
  edm::EDGetTokenT<reco::VertexCollection> theVertexToken;
  edm::InputTag theInputTrkTag;
  edm::EDGetTokenT<reco::TrackCollection> theInputTrkToken;

  bool useVtxTks;

  double thePtMin;
  double theOriginRadius;
  double theOriginHalfLength;
  double theOriginZPos;

  double theDeltaEta;
  double theDeltaPhi;
  edm::EDGetTokenT<MeasurementTrackerEvent> theMeasurementTrackerToken;
  RectangularEtaPhiTrackingRegion::UseMeasurementTracker m_howToUseMeasurementTracker;
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> theFieldToken;
  edm::ESGetToken<MultipleScatteringParametrisationMaker, TrackerMultipleScatteringRecord> theMSMakerToken;
  bool m_searchOpt;
};

#endif
