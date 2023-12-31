#ifndef Integration_ProducerWithPSetDesc_h
#define Integration_ProducerWithPSetDesc_h

// Used to test the ParameterSetDescription.
// This module has a description with many
// different types and values of parameters,
// including nested ParameterSets and vectors
// of them.

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

namespace edm {
  class ConfigurationDescriptions;
}

namespace edmtest {
  class ProducerWithPSetDesc : public edm::global::EDProducer<> {
  public:
    explicit ProducerWithPSetDesc(edm::ParameterSet const& ps);

    void produce(edm::StreamID, edm::Event& e, edm::EventSetup const& c) const override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    bool testingAutoGeneratedCfi;
  };
}  // namespace edmtest
#endif
