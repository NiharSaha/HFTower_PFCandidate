// system include files
#include <memory>
#include <string>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalRecHit/interface/HFRecHit.h"
#include "DataFormats/HcalRecHit/interface/HORecHit.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include <iostream>

//
// class declaration
//

class AlCaGammaJetProducer : public edm::global::EDProducer<> {
public:
  explicit AlCaGammaJetProducer(const edm::ParameterSet&);
  ~AlCaGammaJetProducer() override = default;
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  bool select(const reco::PhotonCollection&, const reco::PFJetCollection&) const;

  // ----------member data ---------------------------

  edm::InputTag labelPhoton_, labelPFJet_, labelHBHE_, labelHF_, labelHO_, labelTrigger_, labelPFCandidate_,
      labelVertex_, labelPFMET_, labelGsfEle_, labelRho_, labelConv_, labelBeamSpot_, labelLoosePhot_, labelTightPhot_;
  double minPtJet_, minPtPhoton_;

  edm::EDGetTokenT<reco::PhotonCollection> tok_Photon_;
  edm::EDGetTokenT<reco::PFJetCollection> tok_PFJet_;
  edm::EDGetTokenT<edm::SortedCollection<HBHERecHit, edm::StrictWeakOrdering<HBHERecHit>>> tok_HBHE_;
  edm::EDGetTokenT<edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit>>> tok_HF_;
  edm::EDGetTokenT<edm::SortedCollection<HORecHit, edm::StrictWeakOrdering<HORecHit>>> tok_HO_;
  edm::EDGetTokenT<edm::TriggerResults> tok_TrigRes_;
  edm::EDGetTokenT<reco::PFCandidateCollection> tok_PFCand_;
  edm::EDGetTokenT<reco::VertexCollection> tok_Vertex_;
  edm::EDGetTokenT<reco::PFMETCollection> tok_PFMET_;
  edm::EDGetTokenT<reco::GsfElectronCollection> tok_GsfElec_;
  edm::EDGetTokenT<double> tok_Rho_;
  edm::EDGetTokenT<reco::ConversionCollection> tok_Conv_;
  edm::EDGetTokenT<reco::BeamSpot> tok_BS_;
  edm::EDGetTokenT<edm::ValueMap<Bool_t>> tok_loosePhoton_;
  edm::EDGetTokenT<edm::ValueMap<Bool_t>> tok_tightPhoton_;

  edm::EDPutTokenT<reco::PhotonCollection> put_photon_;
  edm::EDPutTokenT<reco::PFJetCollection> put_pfJet_;
  edm::EDPutTokenT<edm::SortedCollection<HBHERecHit, edm::StrictWeakOrdering<HBHERecHit>>> put_hbhe_;
  edm::EDPutTokenT<edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit>>> put_hf_;
  edm::EDPutTokenT<edm::SortedCollection<HORecHit, edm::StrictWeakOrdering<HORecHit>>> put_ho_;
  edm::EDPutTokenT<edm::TriggerResults> put_trigger_;
  edm::EDPutTokenT<std::vector<Bool_t>> put_loosePhot_;
  edm::EDPutTokenT<std::vector<Bool_t>> put_tightPhot_;
  edm::EDPutTokenT<double> put_rho_;
  edm::EDPutTokenT<reco::PFCandidateCollection> put_pfCandidate_;
  edm::EDPutTokenT<reco::VertexCollection> put_vertex_;
  edm::EDPutTokenT<reco::PFMETCollection> put_pfMET_;
  edm::EDPutTokenT<reco::GsfElectronCollection> put_gsfEle_;
  edm::EDPutTokenT<reco::ConversionCollection> put_conv_;
  edm::EDPutTokenT<reco::BeamSpot> put_beamSpot_;
};

AlCaGammaJetProducer::AlCaGammaJetProducer(const edm::ParameterSet& iConfig) {
  // Take input
  labelPhoton_ = iConfig.getParameter<edm::InputTag>("PhoInput");
  labelPFJet_ = iConfig.getParameter<edm::InputTag>("PFjetInput");
  labelHBHE_ = iConfig.getParameter<edm::InputTag>("HBHEInput");
  labelHF_ = iConfig.getParameter<edm::InputTag>("HFInput");
  labelHO_ = iConfig.getParameter<edm::InputTag>("HOInput");
  labelTrigger_ = iConfig.getParameter<edm::InputTag>("TriggerResults");
  labelPFCandidate_ = iConfig.getParameter<edm::InputTag>("particleFlowInput");
  labelVertex_ = iConfig.getParameter<edm::InputTag>("VertexInput");
  labelPFMET_ = iConfig.getParameter<edm::InputTag>("METInput");
  labelGsfEle_ = iConfig.getParameter<edm::InputTag>("gsfeleInput");
  labelRho_ = iConfig.getParameter<edm::InputTag>("rhoInput");
  labelConv_ = iConfig.getParameter<edm::InputTag>("ConversionsInput");
  labelBeamSpot_ = iConfig.getParameter<edm::InputTag>("BeamSpotInput");
  labelLoosePhot_ = iConfig.getParameter<edm::InputTag>("PhoLoose");
  labelTightPhot_ = iConfig.getParameter<edm::InputTag>("PhoTight");
  minPtJet_ = iConfig.getParameter<double>("MinPtJet");
  minPtPhoton_ = iConfig.getParameter<double>("MinPtPhoton");

  tok_Photon_ = consumes<reco::PhotonCollection>(labelPhoton_);
  tok_PFJet_ = consumes<reco::PFJetCollection>(labelPFJet_);
  tok_HBHE_ = consumes<edm::SortedCollection<HBHERecHit, edm::StrictWeakOrdering<HBHERecHit>>>(labelHBHE_);
  tok_HF_ = consumes<edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit>>>(labelHF_);
  tok_HO_ = consumes<edm::SortedCollection<HORecHit, edm::StrictWeakOrdering<HORecHit>>>(labelHO_);
  tok_TrigRes_ = consumes<edm::TriggerResults>(labelTrigger_);
  tok_PFCand_ = consumes<reco::PFCandidateCollection>(labelPFCandidate_);
  tok_Vertex_ = consumes<reco::VertexCollection>(labelVertex_);
  tok_PFMET_ = consumes<reco::PFMETCollection>(labelPFMET_);
  tok_loosePhoton_ = consumes<edm::ValueMap<Bool_t>>(labelLoosePhot_);
  tok_tightPhoton_ = consumes<edm::ValueMap<Bool_t>>(labelTightPhot_);
  tok_GsfElec_ = consumes<reco::GsfElectronCollection>(labelGsfEle_);
  tok_Rho_ = consumes<double>(labelRho_);
  tok_Conv_ = consumes<reco::ConversionCollection>(labelConv_);
  tok_BS_ = consumes<reco::BeamSpot>(labelBeamSpot_);

  // register your products
  put_photon_ = produces<reco::PhotonCollection>(labelPhoton_.encode());
  put_pfJet_ = produces<reco::PFJetCollection>(labelPFJet_.encode());
  put_hbhe_ = produces<edm::SortedCollection<HBHERecHit, edm::StrictWeakOrdering<HBHERecHit>>>(labelHBHE_.encode());
  put_hf_ = produces<edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit>>>(labelHF_.encode());
  put_ho_ = produces<edm::SortedCollection<HORecHit, edm::StrictWeakOrdering<HORecHit>>>(labelHO_.encode());
  put_trigger_ = produces<edm::TriggerResults>(labelTrigger_.encode());
  put_loosePhot_ = produces<std::vector<Bool_t>>(labelLoosePhot_.encode());
  put_tightPhot_ = produces<std::vector<Bool_t>>(labelTightPhot_.encode());
  put_rho_ = produces<double>(labelRho_.encode());
  put_pfCandidate_ = produces<reco::PFCandidateCollection>(labelPFCandidate_.encode());
  put_vertex_ = produces<reco::VertexCollection>(labelVertex_.encode());
  put_pfMET_ = produces<reco::PFMETCollection>(labelPFMET_.encode());
  put_gsfEle_ = produces<reco::GsfElectronCollection>(labelGsfEle_.encode());
  put_conv_ = produces<reco::ConversionCollection>(labelConv_.encode());
  put_beamSpot_ = produces<reco::BeamSpot>(labelBeamSpot_.encode());
}

bool AlCaGammaJetProducer::select(const reco::PhotonCollection& ph, const reco::PFJetCollection& jt) const {
  // Check the requirement for minimum pT
  if (ph.empty())
    return false;
  bool ok(false);
  for (reco::PFJetCollection::const_iterator itr = jt.begin(); itr != jt.end(); ++itr) {
    if (itr->pt() >= minPtJet_) {
      ok = true;
      break;
    }
  }
  if (!ok)
    return ok;
  for (reco::PhotonCollection::const_iterator itr = ph.begin(); itr != ph.end(); ++itr) {
    if (itr->pt() >= minPtPhoton_)
      return ok;
  }
  return false;
}

// ------------ method called to produce the data  ------------
void AlCaGammaJetProducer::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup&) const {
  // Access the collections from iEvent
  auto const& phoHandle = iEvent.getHandle(tok_Photon_);
  if (!phoHandle.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get the product " << labelPhoton_;
    return;
  }
  const reco::PhotonCollection& photon = *(phoHandle.product());

  auto const& pfjet = iEvent.getHandle(tok_PFJet_);
  if (!pfjet.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelPFJet_;
    return;
  }
  const reco::PFJetCollection& pfjets = *(pfjet.product());

  auto const& pfc = iEvent.getHandle(tok_PFCand_);
  if (!pfc.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelPFCandidate_;
    return;
  }
  const reco::PFCandidateCollection& pfcand = *(pfc.product());

  auto const& vt = iEvent.getHandle(tok_Vertex_);
  if (!vt.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelVertex_;
    return;
  }
  const reco::VertexCollection& vtx = *(vt.product());

  auto const& pfmt = iEvent.getHandle(tok_PFMET_);
  if (!pfmt.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelPFMET_;
    return;
  }
  const reco::PFMETCollection& pfmet = *(pfmt.product());

  auto const& hbhe = iEvent.getHandle(tok_HBHE_);
  if (!hbhe.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelHBHE_;
    return;
  }
  const edm::SortedCollection<HBHERecHit, edm::StrictWeakOrdering<HBHERecHit>>& Hithbhe = *(hbhe.product());

  auto const& ho = iEvent.getHandle(tok_HO_);
  if (!ho.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelHO_;
    return;
  }
  const edm::SortedCollection<HORecHit, edm::StrictWeakOrdering<HORecHit>>& Hitho = *(ho.product());

  auto const& hf = iEvent.getHandle(tok_HF_);
  if (!hf.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelHF_;
    return;
  }
  const edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit>>& Hithf = *(hf.product());

  auto const& trig = iEvent.getHandle(tok_TrigRes_);
  if (!trig.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelTrigger_;
    return;
  }
  const edm::TriggerResults& trigres = *(trig.product());

  auto const& rh = iEvent.getHandle(tok_Rho_);
  if (!rh.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelRho_;
    return;
  }
  const double rho_val = *(rh.product());

  auto const& gsf = iEvent.getHandle(tok_GsfElec_);
  if (!gsf.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelGsfEle_;
    return;
  }
  const reco::GsfElectronCollection& gsfele = *(gsf.product());

  auto const& con = iEvent.getHandle(tok_Conv_);
  if (!con.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelConv_;
    return;
  }
  const reco::ConversionCollection& conv = *(con.product());

  auto const& bs = iEvent.getHandle(tok_BS_);
  if (!bs.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelBeamSpot_;
    return;
  }
  const reco::BeamSpot& beam = *(bs.product());

  // declare variables
  // copy from standard place, if the event is useful
  reco::BeamSpot miniBeamSpotCollection(
      beam.position(), beam.sigmaZ(), beam.dxdz(), beam.dydz(), beam.BeamWidthX(), beam.covariance(), beam.type());

  std::vector<Bool_t> miniLoosePhoton{};
  std::vector<Bool_t> miniTightPhoton{};

  // See if this event is useful
  bool accept = select(photon, pfjets);
  if (accept) {
    iEvent.emplace(put_photon_, photon);
    iEvent.emplace(put_pfJet_, pfjets);
    iEvent.emplace(put_hbhe_, Hithbhe);
    iEvent.emplace(put_hf_, Hithf);
    iEvent.emplace(put_ho_, Hitho);
    iEvent.emplace(put_trigger_, trigres);
    iEvent.emplace(put_pfCandidate_, pfcand);
    iEvent.emplace(put_vertex_, vtx);
    iEvent.emplace(put_pfMET_, pfmet);
    iEvent.emplace(put_gsfEle_, gsfele);
    iEvent.emplace(put_rho_, rho_val);
    iEvent.emplace(put_conv_, conv);

    auto const& loosePhotonQual = iEvent.getHandle(tok_loosePhoton_);
    auto const& tightPhotonQual = iEvent.getHandle(tok_tightPhoton_);
    if (loosePhotonQual.isValid() && tightPhotonQual.isValid()) {
      miniLoosePhoton.reserve(photon.size());
      miniTightPhoton.reserve(photon.size());
      for (int iPho = 0; iPho < int(photon.size()); ++iPho) {
        edm::Ref<reco::PhotonCollection> photonRef(phoHandle, iPho);
        if (!photonRef) {
          edm::LogVerbatim("AlCaGammaJet") << "failed ref";
          miniLoosePhoton.push_back(-1);
          miniTightPhoton.push_back(-1);
        } else {
          miniLoosePhoton.push_back((*loosePhotonQual)[photonRef]);
          miniTightPhoton.push_back((*tightPhotonQual)[photonRef]);
        }
      }
    }
  } else {
    iEvent.emplace(put_photon_, reco::PhotonCollection{});
    iEvent.emplace(put_pfJet_, reco::PFJetCollection{});
    iEvent.emplace(put_hbhe_, edm::SortedCollection<HBHERecHit, edm::StrictWeakOrdering<HBHERecHit>>{});
    iEvent.emplace(put_hf_, edm::SortedCollection<HFRecHit, edm::StrictWeakOrdering<HFRecHit>>{});
    iEvent.emplace(put_ho_, edm::SortedCollection<HORecHit, edm::StrictWeakOrdering<HORecHit>>{});
    iEvent.emplace(put_trigger_, edm::TriggerResults{});
    iEvent.emplace(put_pfCandidate_, reco::PFCandidateCollection{});
    iEvent.emplace(put_vertex_, reco::VertexCollection{});
    iEvent.emplace(put_pfMET_, reco::PFMETCollection{});
    iEvent.emplace(put_gsfEle_, reco::GsfElectronCollection{});
    iEvent.emplace(put_rho_, double{});
    iEvent.emplace(put_conv_, reco::ConversionCollection{});
  }

  //Put them in the event
  iEvent.emplace(put_beamSpot_, std::move(miniBeamSpotCollection));
  iEvent.emplace(put_loosePhot_, std::move(miniLoosePhoton));
  iEvent.emplace(put_tightPhot_, std::move(miniTightPhoton));

  return;
}

void AlCaGammaJetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("PhoInput", edm::InputTag("gedPhotons"));
  desc.add<edm::InputTag>("PFjetInput", edm::InputTag("ak4PFJetsCHS"));
  desc.add<edm::InputTag>("HBHEInput", edm::InputTag("hbhereco"));
  desc.add<edm::InputTag>("HFInput", edm::InputTag("hfreco"));
  desc.add<edm::InputTag>("HOInput", edm::InputTag("horeco"));
  desc.add<edm::InputTag>("METInput", edm::InputTag("pfMet"));
  desc.add<edm::InputTag>("TriggerResults", edm::InputTag("TriggerResults::HLT"));
  desc.add<edm::InputTag>("gsfeleInput", edm::InputTag("gedGsfElectrons"));
  desc.add<edm::InputTag>("particleFlowInput", edm::InputTag("particleFlow"));
  desc.add<edm::InputTag>("VertexInput", edm::InputTag("offlinePrimaryVertices"));
  desc.add<edm::InputTag>("ConversionsInput", edm::InputTag("allConversions"));
  desc.add<edm::InputTag>("rhoInput", edm::InputTag("fixedGridRhoFastjetAll"));
  desc.add<edm::InputTag>("BeamSpotInput", edm::InputTag("offlineBeamSpot"));
  desc.add<edm::InputTag>("PhoLoose", edm::InputTag("PhotonIDProdGED", "PhotonCutBasedIDLoose"));
  desc.add<edm::InputTag>("PhoTight", edm::InputTag("PhotonIDProdGED", "PhotonCutBasedIDTight"));
  desc.add<double>("MinPtJet", 10.0);
  desc.add<double>("MinPtPhoton", 10.0);
  descriptions.add("alcaGammaJetProducer", desc);
}

DEFINE_FWK_MODULE(AlCaGammaJetProducer);
