
/* 
 * class TauDiscriminationAgainstCaloMuon
 * created : Nov 20 2010
 * revised : 
 * Authors : Christian Veelken (UC Davis)
 */

#include "RecoTauTag/RecoTau/interface/TauDiscriminationProducerBase.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include <FWCore/ParameterSet/interface/ConfigurationDescriptions.h>
#include <FWCore/ParameterSet/interface/ParameterSetDescription.h>

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/CaloTowers/interface/CaloTower.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateClosestToPoint.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/Math/interface/deltaR.h"

#include <TVector3.h>
#include <TMath.h>

#include <string>

namespace {

  using namespace reco;

  // define acess to lead. track for CaloTaus
  template <typename T>
  class TauLeadTrackExtractor {
  public:
    reco::TrackRef getLeadTrack(const T& tau) const { return tau.leadTrack(); }
    double getTrackPtSum(const T& tau) const {
      double trackPtSum = 0.;
      for (TrackRefVector::const_iterator signalTrack = tau.signalTracks().begin();
           signalTrack != tau.signalTracks().end();
           ++signalTrack) {
        trackPtSum += (*signalTrack)->pt();
      }
      return trackPtSum;
    }
  };

  // define acess to lead. track for PFTaus
  template <>
  class TauLeadTrackExtractor<reco::PFTau> {
  public:
    reco::TrackRef getLeadTrack(const reco::PFTau& tau) const { return tau.leadPFChargedHadrCand()->trackRef(); }
    double getTrackPtSum(const reco::PFTau& tau) const {
      double trackPtSum = 0.;
      for (std::vector<CandidatePtr>::const_iterator signalTrack = tau.signalChargedHadrCands().begin();
           signalTrack != tau.signalChargedHadrCands().end();
           ++signalTrack) {
        trackPtSum += (*signalTrack)->pt();
      }
      return trackPtSum;
    }
  };

  template <class TauType, class TauDiscriminator>
  class TauDiscriminationAgainstCaloMuon final : public TauDiscriminationProducerBase<TauType, TauDiscriminator> {
  public:
    // setup framework types for this tautype
    typedef std::vector<TauType> TauCollection;
    typedef edm::Ref<TauCollection> TauRef;

    explicit TauDiscriminationAgainstCaloMuon(const edm::ParameterSet&);
    ~TauDiscriminationAgainstCaloMuon() override {}

    // called at the beginning of every event
    void beginEvent(const edm::Event&, const edm::EventSetup&) override;

    double discriminate(const TauRef&) const override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    edm::InputTag srcEcalRecHitsBarrel_;
    edm::Handle<EcalRecHitCollection> ebRecHits_;
    edm::InputTag srcEcalRecHitsEndcap_;
    edm::Handle<EcalRecHitCollection> eeRecHits_;
    edm::InputTag srcHcalRecHits_;
    edm::Handle<HBHERecHitCollection> hbheRecHits_;

    edm::InputTag srcVertex_;
    GlobalPoint eventVertexPosition_;

    const TransientTrackBuilder* trackBuilder_;
    const CaloGeometry* caloGeometry_;
    const edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> trackBuilderToken_;
    const edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeometryToken_;

    TauLeadTrackExtractor<TauType> leadTrackExtractor_;

    double minLeadTrackPt_;
    double minLeadTrackPtFraction_;

    double drEcal_;
    double drHcal_;

    double maxEnEcal_;
    double maxEnHcal_;

    double maxEnToTrackRatio_;
  };

  template <class TauType, class TauDiscriminator>
  TauDiscriminationAgainstCaloMuon<TauType, TauDiscriminator>::TauDiscriminationAgainstCaloMuon(
      const edm::ParameterSet& cfg)
      : TauDiscriminationProducerBase<TauType, TauDiscriminator>(cfg),
        trackBuilderToken_(this->esConsumes(edm::ESInputTag("", "TransientTrackBuilder"))),
        caloGeometryToken_(this->esConsumes()) {
    srcEcalRecHitsBarrel_ = cfg.getParameter<edm::InputTag>("srcEcalRecHitsBarrel");
    srcEcalRecHitsEndcap_ = cfg.getParameter<edm::InputTag>("srcEcalRecHitsEndcap");
    srcHcalRecHits_ = cfg.getParameter<edm::InputTag>("srcHcalRecHits");

    srcVertex_ = cfg.getParameter<edm::InputTag>("srcVertex");

    minLeadTrackPt_ = cfg.getParameter<double>("minLeadTrackPt");
    minLeadTrackPtFraction_ = cfg.getParameter<double>("minLeadTrackPtFraction");

    drEcal_ = cfg.getParameter<double>("dRecal");
    drHcal_ = cfg.getParameter<double>("dRhcal");

    maxEnEcal_ = cfg.getParameter<double>("maxEnEcal");
    maxEnHcal_ = cfg.getParameter<double>("maxEnHcal");

    maxEnToTrackRatio_ = cfg.getParameter<double>("maxEnToTrackRatio");
  }

  template <class TauType, class TauDiscriminator>
  void TauDiscriminationAgainstCaloMuon<TauType, TauDiscriminator>::beginEvent(const edm::Event& evt,
                                                                               const edm::EventSetup& evtSetup) {
    evt.getByLabel(srcEcalRecHitsBarrel_, ebRecHits_);
    evt.getByLabel(srcEcalRecHitsEndcap_, eeRecHits_);
    evt.getByLabel(srcHcalRecHits_, hbheRecHits_);

    trackBuilder_ = &evtSetup.getData(trackBuilderToken_);
    if (!trackBuilder_) {
      edm::LogError("TauDiscriminationAgainstCaloMuon::discriminate") << " Failed to access TransientTrackBuilder !!";
    }

    caloGeometry_ = &evtSetup.getData(caloGeometryToken_);
    if (!caloGeometry_) {
      edm::LogError("TauDiscriminationAgainstCaloMuon::discriminate") << " Failed to access CaloGeometry !!";
    }

    edm::Handle<reco::VertexCollection> vertices;
    evt.getByLabel(srcVertex_, vertices);
    eventVertexPosition_ = GlobalPoint(0., 0., 0.);
    if (!vertices->empty()) {
      const reco::Vertex& thePrimaryEventVertex = (*vertices->begin());
      eventVertexPosition_ =
          GlobalPoint(thePrimaryEventVertex.x(), thePrimaryEventVertex.y(), thePrimaryEventVertex.z());
    }
  }

  double compEcalEnergySum(const EcalRecHitCollection& ecalRecHits,
                           const CaloSubdetectorGeometry* detGeometry,
                           const reco::TransientTrack& transientTrack,
                           double dR,
                           const GlobalPoint& eventVertexPosition) {
    double ecalEnergySum = 0.;
    for (EcalRecHitCollection::const_iterator ecalRecHit = ecalRecHits.begin(); ecalRecHit != ecalRecHits.end();
         ++ecalRecHit) {
      auto cellGeometry = detGeometry->getGeometry(ecalRecHit->detid());

      if (!cellGeometry) {
        edm::LogError("compEcalEnergySum")
            << " Failed to access ECAL geometry for detId = " << ecalRecHit->detid().rawId() << " --> skipping !!";
        continue;
      }

      const GlobalPoint& cellPosition = cellGeometry->getPosition();

      //--- CV: speed up computation by requiring eta-phi distance
      //        between cell position and track direction to be dR < 0.5
      Vector3DBase<float, GlobalTag> cellPositionRelVertex = (cellPosition)-eventVertexPosition;
      if (deltaR(cellPositionRelVertex.eta(),
                 cellPositionRelVertex.phi(),
                 transientTrack.track().eta(),
                 transientTrack.track().phi()) > 0.5)
        continue;

      TrajectoryStateClosestToPoint dcaPosition = transientTrack.trajectoryStateClosestToPoint(cellPosition);

      Vector3DBase<float, GlobalTag> d = (cellPosition - dcaPosition.position());

      TVector3 d3(d.x(), d.y(), d.z());
      TVector3 dir(transientTrack.track().px(), transientTrack.track().py(), transientTrack.track().pz());

      double dPerp = d3.Cross(dir.Unit()).Mag();
      double dParl = TVector3(cellPosition.x(), cellPosition.y(), cellPosition.z()).Dot(dir.Unit());

      if (dPerp < dR && dParl > 100.) {
        ecalEnergySum += ecalRecHit->energy();
      }
    }

    return ecalEnergySum;
  }

  double compHcalEnergySum(const HBHERecHitCollection& hcalRecHits,
                           const HcalGeometry* hcGeometry,
                           const reco::TransientTrack& transientTrack,
                           double dR,
                           const GlobalPoint& eventVertexPosition) {
    double hcalEnergySum = 0.;
    for (HBHERecHitCollection::const_iterator hcalRecHit = hcalRecHits.begin(); hcalRecHit != hcalRecHits.end();
         ++hcalRecHit) {
      const GlobalPoint cellPosition = hcGeometry->getPosition(hcalRecHit->detid());

      //--- CV: speed up computation by requiring eta-phi distance
      //        between cell position and track direction to be dR < 0.5
      Vector3DBase<float, GlobalTag> cellPositionRelVertex = (cellPosition)-eventVertexPosition;
      if (deltaR(cellPositionRelVertex.eta(),
                 cellPositionRelVertex.phi(),
                 transientTrack.track().eta(),
                 transientTrack.track().phi()) > 0.5)
        continue;

      TrajectoryStateClosestToPoint dcaPosition = transientTrack.trajectoryStateClosestToPoint(cellPosition);

      Vector3DBase<float, GlobalTag> d = ((cellPosition)-dcaPosition.position());

      TVector3 d3(d.x(), d.y(), d.z());
      TVector3 dir(transientTrack.track().px(), transientTrack.track().py(), transientTrack.track().pz());

      double dPerp = d3.Cross(dir.Unit()).Mag();
      double dParl = TVector3(cellPosition.x(), cellPosition.y(), cellPosition.z()).Dot(dir.Unit());

      if (dPerp < dR && dParl > 100.) {
        hcalEnergySum += hcalRecHit->energy();
      }
    }

    return hcalEnergySum;
  }

  template <class TauType, class TauDiscriminator>
  double TauDiscriminationAgainstCaloMuon<TauType, TauDiscriminator>::discriminate(const TauRef& tau) const {
    if (!(trackBuilder_ && caloGeometry_))
      return 0.;

    const CaloSubdetectorGeometry* ebGeometry = caloGeometry_->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
    const CaloSubdetectorGeometry* eeGeometry = caloGeometry_->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
    const HcalGeometry* hcGeometry =
        static_cast<const HcalGeometry*>(caloGeometry_->getSubdetectorGeometry(DetId::Hcal, HcalBarrel));

    TrackRef leadTrackRef = leadTrackExtractor_.getLeadTrack(*tau);

    if ((leadTrackRef.isAvailable() || leadTrackRef.isTransient()) && leadTrackRef.isNonnull()) {
      double leadTrackPt = leadTrackRef->pt();
      double trackPtSum = leadTrackExtractor_.getTrackPtSum(*tau);

      double leadTrackPtFraction = (trackPtSum > 0.) ? (leadTrackPt / trackPtSum) : -1.;

      if (leadTrackPt > minLeadTrackPt_ && leadTrackPtFraction > minLeadTrackPtFraction_) {
        reco::TransientTrack transientTrack = trackBuilder_->build(leadTrackRef);

        double ebEnergySum = compEcalEnergySum(*ebRecHits_, ebGeometry, transientTrack, drEcal_, eventVertexPosition_);
        double eeEnergySum = compEcalEnergySum(*eeRecHits_, eeGeometry, transientTrack, drEcal_, eventVertexPosition_);
        double ecalEnergySum = ebEnergySum + eeEnergySum;

        double hbheEnergySum =
            compHcalEnergySum(*hbheRecHits_, hcGeometry, transientTrack, drHcal_, eventVertexPosition_);

        double caloEnergySum = ecalEnergySum + hbheEnergySum;

        if (ecalEnergySum < maxEnEcal_ && hbheEnergySum < maxEnHcal_ &&
            caloEnergySum < (maxEnToTrackRatio_ * leadTrackPt))
          return 0.;
      }
    }

    return 1.;
  }

}  // namespace

#include "FWCore/Framework/interface/MakerMacros.h"

typedef TauDiscriminationAgainstCaloMuon<PFTau, PFTauDiscriminator> PFRecoTauDiscriminationAgainstCaloMuon;

// accordingly method for specific class
template <>
void TauDiscriminationAgainstCaloMuon<PFTau, PFTauDiscriminator>::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions) {
  // pfRecoTauDiscriminationAgainstCaloMuon
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("srcHcalRecHits", edm::InputTag("hbhereco"));
  desc.add<double>("minLeadTrackPt", 15.0);
  desc.add<double>("maxEnToTrackRatio", 0.25);
  desc.add<edm::InputTag>("srcVertex", edm::InputTag("offlinePrimaryVertices"));
  desc.add<edm::InputTag>("PFTauProducer", edm::InputTag("pfRecoTauProducer"));
  desc.add<edm::InputTag>("srcEcalRecHitsBarrel", edm::InputTag("ecalRecHit", "EcalRecHitsEB"));
  desc.add<double>("dRhcal", 25.0);
  {
    edm::ParameterSetDescription psd0;
    psd0.add<std::string>("BooleanOperator", "and");
    {
      edm::ParameterSetDescription psd1;
      psd1.add<double>("cut");
      psd1.add<edm::InputTag>("Producer");
      psd0.addOptional<edm::ParameterSetDescription>("leadTrack", psd1);
    }
    desc.add<edm::ParameterSetDescription>("Prediscriminants", psd0);
  }
  desc.add<double>("maxEnHcal", 8.0);
  desc.add<double>("dRecal", 15.0);
  desc.add<edm::InputTag>("srcEcalRecHitsEndcap", edm::InputTag("ecalRecHit", "EcalRecHitsEE"));
  desc.add<double>("minLeadTrackPtFraction", 0.8);
  desc.add<double>("maxEnEcal", 3.0);
  descriptions.add("pfRecoTauDiscriminationAgainstCaloMuon", desc);
}

DEFINE_FWK_MODULE(PFRecoTauDiscriminationAgainstCaloMuon);
