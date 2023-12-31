/*
 *  See header file for a description of this class.
 *
 *  \author N. Amapane & G. Cerminara - INFN Torino
 */

#include "RecoLocalMuon/DTRecHit/interface/DTRecHitBaseAlgo.h"

#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "CalibMuon/DTDigiSync/interface/DTTTrigBaseSync.h"
#include "CalibMuon/DTDigiSync/interface/DTTTrigSyncFactory.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

using namespace std;
using namespace edm;

DTRecHitBaseAlgo::DTRecHitBaseAlgo(const ParameterSet& config, ConsumesCollector cc)
    : theSync{DTTTrigSyncFactory::get()->create(
          config.getParameter<string>("tTrigMode"), config.getParameter<ParameterSet>("tTrigModeConfig"), cc)} {}

DTRecHitBaseAlgo::~DTRecHitBaseAlgo() {}

// Build all hits in the range associated to the layerId, at the 1st step.
OwnVector<DTRecHit1DPair> DTRecHitBaseAlgo::reconstruct(const DTLayer* layer,
                                                        const DTLayerId& layerId,
                                                        const DTDigiCollection::Range& digiRange) {
  OwnVector<DTRecHit1DPair> result;

  // Loop over all digis in the given range
  for (DTDigiCollection::const_iterator digi = digiRange.first; digi != digiRange.second; digi++) {
    // Get the wireId
    DTWireId wireId(layerId, (*digi).wire());

    LocalError tmpErr;
    LocalPoint lpoint, rpoint;
    // Call the compute method
    bool OK = compute(layer, *digi, lpoint, rpoint, tmpErr);
    if (!OK)
      continue;

    // Build a new pair of 1D rechit
    auto recHitPair = std::make_unique<DTRecHit1DPair>(wireId, *digi);

    // Set the position and the error of the 1D rechits
    recHitPair->setPositionAndError(DTEnums::Left, lpoint, tmpErr);
    recHitPair->setPositionAndError(DTEnums::Right, rpoint, tmpErr);

    result.push_back(std::move(recHitPair));
  }
  return result;
}
