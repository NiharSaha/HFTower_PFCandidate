// -*- C++ -*-
//
// Package:    SecSourceAnalyzer
// Class:      SecSourceAnalyzer
//
/**\class SecSourceAnalyzer SecSourceAnalyzer.cc SecSource/SecSourceAnalyzer/src/SecSourceAnalyzer.cc
*/
//
// Original Author:  Emilia Lubenova Becheva
//         Created:  Wed Apr 22 16:54:31 CEST 2009
//
//

#ifndef SecSourceAnalyzer_h
#define SecSourceAnalyzer_h

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Provenance/interface/EventID.h"

#include "Mixing/Base/interface/PileUp.h"

//
// class declaration
//
namespace edm {

  class ModuleCallingContext;

  class SecSourceAnalyzer : public edm::one::EDAnalyzer<> {
  public:
    explicit SecSourceAnalyzer(const edm::ParameterSet&);
    ~SecSourceAnalyzer() override;

    bool getBranches(EventPrincipal const& ep, ModuleCallingContext const*);
    bool dummyFunction(EventPrincipal const& ep) { return true; }

  private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override;

    // ----------member data ---------------------------
    int minBunch_;
    int maxBunch_;

    bool dataStep2_;
    edm::InputTag label_;

    std::vector<std::vector<edm::SecondaryEventIDAndFileInfo> > vectorEventIDs_;

    std::unique_ptr<PileUp> input_;
    std::vector<float> TrueNumInteractions_[5];

    InputTag tag_;
  };
}  // namespace edm
#endif
