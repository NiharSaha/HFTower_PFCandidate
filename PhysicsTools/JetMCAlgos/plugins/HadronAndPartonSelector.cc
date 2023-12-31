// -*- C++ -*-
//
// Package:    HadronAndPartonSelector
// Class:      HadronAndPartonSelector
//
/**\class HadronAndPartonSelector HadronAndPartonSelector.cc PhysicsTools/JetMCAlgos/plugins/HadronAndPartonSelector.cc
 * \brief Selects hadrons and partons from a collection of GenParticles
 *
 * This producer selects hadrons, partons, and leptons from a collection of GenParticles and stores vectors of EDM references
 * to these particles in the event. The following hadrons are selected:
 *
 * - b hadrons that do not have other b hadrons as daughters
 *
 * - c hadrons that do not have other c hadrons as daughters
 *
 * Older Fortran Monte Carlo generators (Pythia6 and Herwig6) follow the HEPEVT [1] particle status code convention while
 * newer C++ Monte Carlo generators (Pythia8, Herwig++, and Sherpa) follow the HepMC [2] particle status code convention.
 * However, both conventions give considerable freedom in defining the status codes of intermediate particle states. Hence,
 * the parton selection is generator-dependent and is described in each of the parton selectors separately.
 *
 * Using the provenance information of the GenEventInfoProduct, the producer attempts to automatically determine what generator
 * was used to hadronize events and based on that information decides what parton selection mode to use. It is also possible
 * to enforce any of the supported parton selection modes.
 *
 * The selected hadrons and partons are finally used by the JetFlavourClustering producer to determine the jet flavour.
 *
 * The following leptons are selected:
 *
 * - status==1 electrons and muons
 *
 * - status==2 taus
 *
 *
 * [1] http://cepa.fnal.gov/psm/stdhep/
 * [2] http://lcgapp.cern.ch/project/simu/HepMC/
 */
//
// Original Author:  Dinko Ferencek
//         Created:  Tue Nov  5 22:43:43 CET 2013
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "FWCore/Common/interface/Provenance.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "PhysicsTools/JetMCUtils/interface/CandMCTag.h"
#include "CommonTools/CandUtils/interface/pdgIdUtils.h"
#include "PhysicsTools/JetMCAlgos/interface/BasePartonSelector.h"
#include "PhysicsTools/JetMCAlgos/interface/Pythia6PartonSelector.h"
#include "PhysicsTools/JetMCAlgos/interface/Pythia8PartonSelector.h"
#include "PhysicsTools/JetMCAlgos/interface/HerwigppPartonSelector.h"
#include "PhysicsTools/JetMCAlgos/interface/SherpaPartonSelector.h"

//
// constants, enums and typedefs
//
typedef std::shared_ptr<BasePartonSelector> PartonSelectorPtr;

//
// class declaration
//

class HadronAndPartonSelector : public edm::stream::EDProducer<> {
public:
  explicit HadronAndPartonSelector(const edm::ParameterSet&);
  ~HadronAndPartonSelector() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  const edm::EDGetTokenT<GenEventInfoProduct> srcToken_;                // To get handronizer module type
  const edm::EDGetTokenT<reco::GenParticleCollection> particlesToken_;  // Input GenParticle collection

  std::string partonMode_;  // Parton selection mode
  bool fullChainPhysPartons_;
  bool partonSelectorSet_;
  PartonSelectorPtr partonSelector_;
};

//
// static data member definitions
//

//
// constructors and destructor
//
HadronAndPartonSelector::HadronAndPartonSelector(const edm::ParameterSet& iConfig)
    :

      srcToken_(mayConsume<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("src"))),
      particlesToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("particles"))),
      partonMode_(iConfig.getParameter<std::string>("partonMode")),
      fullChainPhysPartons_(iConfig.getParameter<bool>("fullChainPhysPartons"))

{
  //register your products
  produces<reco::GenParticleRefVector>("bHadrons");
  produces<reco::GenParticleRefVector>("cHadrons");
  produces<reco::GenParticleRefVector>("algorithmicPartons");
  produces<reco::GenParticleRefVector>("physicsPartons");
  produces<reco::GenParticleRefVector>("leptons");

  partonSelectorSet_ = false;
  partonSelector_ = nullptr;
}

HadronAndPartonSelector::~HadronAndPartonSelector() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void HadronAndPartonSelector::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // determine hadronizer type (done only once per job)
  if (partonMode_ == "Auto") {
    edm::Handle<GenEventInfoProduct> genEvtInfoProduct;
    iEvent.getByToken(srcToken_, genEvtInfoProduct);

    std::string moduleName = "";
    if (genEvtInfoProduct.isValid()) {
      const edm::StableProvenance& prov = iEvent.getStableProvenance(genEvtInfoProduct.id());
      moduleName = edm::moduleName(prov, iEvent.processHistory());
      if (moduleName == "ExternalGeneratorFilter") {
        moduleName = edm::parameterSet(prov, iEvent.processHistory()).getParameter<std::string>("@external_type");
        edm::LogInfo("SpecialModule") << "GEN events are produced by ExternalGeneratorFilter, "
                                      << "which is a wrapper of the original module: " << moduleName;
      }
    }

    if (moduleName.find("Pythia6") != std::string::npos)
      partonMode_ = "Pythia6";
    else if (moduleName.find("Pythia8") != std::string::npos)
      partonMode_ = "Pythia8";
    else if (moduleName.find("ThePEG") != std::string::npos)
      partonMode_ = "Herwig++";
    else if (moduleName.find("Herwig7") != std::string::npos)
      partonMode_ = "Herwig++";
    else if (moduleName.find("Sherpa") != std::string::npos)
      partonMode_ = "Sherpa";
    else
      partonMode_ = "Undefined";
  }

  // set the parton selection mode (done only once per job)
  if (!partonSelectorSet_) {
    if (partonMode_ == "Undefined")
      edm::LogWarning("UndefinedPartonMode")
          << "Could not automatically determine the hadronizer type and set the correct parton selection mode. "
             "Parton-based jet flavour will not be defined.";
    else if (partonMode_ == "Pythia6") {
      partonSelector_ = PartonSelectorPtr(new Pythia6PartonSelector());
      edm::LogInfo("PartonModeDefined") << "Using Pythia6 parton selection mode.";
    } else if (partonMode_ == "Pythia8") {
      partonSelector_ = PartonSelectorPtr(new Pythia8PartonSelector());
      edm::LogInfo("PartonModeDefined") << "Using Pythia8 parton selection mode.";
    } else if (partonMode_ == "Herwig++") {
      partonSelector_ = PartonSelectorPtr(new HerwigppPartonSelector());
      edm::LogInfo("PartonModeDefined") << "Using Herwig++ parton selection mode.";
    } else if (partonMode_ == "Sherpa") {
      partonSelector_ = PartonSelectorPtr(new SherpaPartonSelector());
      edm::LogInfo("PartonModeDefined") << "Using Sherpa parton selection mode.";
    } else
      throw cms::Exception("InvalidPartonMode") << "Parton selection mode is invalid: " << partonMode_
                                                << ", use Auto | Pythia6 | Pythia8 | Herwig++ | Sherpa" << std::endl;

    partonSelectorSet_ = true;
  }

  edm::Handle<reco::GenParticleCollection> particles;
  iEvent.getByToken(particlesToken_, particles);

  auto bHadrons = std::make_unique<reco::GenParticleRefVector>();
  auto cHadrons = std::make_unique<reco::GenParticleRefVector>();
  auto partons = std::make_unique<reco::GenParticleRefVector>();
  auto physicsPartons = std::make_unique<reco::GenParticleRefVector>();
  auto leptons = std::make_unique<reco::GenParticleRefVector>();

  // loop over particles and select b and c hadrons and leptons
  for (reco::GenParticleCollection::const_iterator it = particles->begin(); it != particles->end(); ++it) {
    // if b hadron
    if (CandMCTagUtils::hasBottom(*it)) {
      // check if any of the daughters is also a b hadron
      bool hasbHadronDaughter = false;
      for (size_t i = 0; i < it->numberOfDaughters(); ++i) {
        if (CandMCTagUtils::hasBottom(*(it->daughter(i)))) {
          hasbHadronDaughter = true;
          break;
        }
      }
      if (hasbHadronDaughter)
        continue;  // skip excited b hadrons that have other b hadrons as daughters

      bHadrons->push_back(reco::GenParticleRef(particles, it - particles->begin()));
    }

    // if c hadron
    if (CandMCTagUtils::hasCharm(*it)) {
      // check if any of the daughters is also a c hadron
      bool hascHadronDaughter = false;
      for (size_t i = 0; i < it->numberOfDaughters(); ++i) {
        if (CandMCTagUtils::hasCharm(*(it->daughter(i)))) {
          hascHadronDaughter = true;
          break;
        }
      }
      if (hascHadronDaughter)
        continue;  // skip excited c hadrons that have other c hadrons as daughters

      cHadrons->push_back(reco::GenParticleRef(particles, it - particles->begin()));
    }

    // status==1 electrons and muons
    if ((reco::isElectron(*it) || reco::isMuon(*it)) && it->status() == 1)
      leptons->push_back(reco::GenParticleRef(particles, it - particles->begin()));

    // status==2 taus
    if (reco::isTau(*it) && it->status() == 2)
      leptons->push_back(reco::GenParticleRef(particles, it - particles->begin()));
  }

  // select algorithmic partons
  if (partonMode_ != "Undefined") {
    partonSelector_->run(particles, partons);
  }

  // select physics partons
  for (reco::GenParticleCollection::const_iterator it = particles->begin(); it != particles->end(); ++it) {
    if (!fullChainPhysPartons_) {
      if (!(it->status() == 3 || ((partonMode_ == "Pythia8") && (it->status() == 23))))
        continue;
    }
    if (!CandMCTagUtils::isParton(*it))
      continue;  // skip particle if not a parton
    physicsPartons->push_back(reco::GenParticleRef(particles, it - particles->begin()));
  }

  iEvent.put(std::move(bHadrons), "bHadrons");
  iEvent.put(std::move(cHadrons), "cHadrons");
  iEvent.put(std::move(partons), "algorithmicPartons");
  iEvent.put(std::move(physicsPartons), "physicsPartons");
  iEvent.put(std::move(leptons), "leptons");
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HadronAndPartonSelector::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HadronAndPartonSelector);
