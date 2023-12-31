#ifndef MultiHitGeneratorFromPairAndLayersFactory_H
#define MultiHitGeneratorFromPairAndLayersFactory_H

#include "RecoTracker/TkSeedGenerator/interface/MultiHitGeneratorFromPairAndLayers.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/PluginManager/interface/PluginFactory.h"

namespace edm {
  class ParameterSet;
}

typedef edmplugin::PluginFactory<MultiHitGeneratorFromPairAndLayers *(const edm::ParameterSet &,
                                                                      edm::ConsumesCollector &)>
    MultiHitGeneratorFromPairAndLayersFactory;

#endif
