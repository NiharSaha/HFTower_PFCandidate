#include "HitExtractorPIX.h"
#include "TrackingTools/DetLayers/interface/DetLayer.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"

#include "FWCore/Framework/interface/EventSetup.h"

#include <iostream>
using namespace ctfseeding;
using namespace std;

HitExtractorPIX::HitExtractorPIX(TrackerDetSide side,
                                 int idLayer,
                                 const std::string& hitProducer,
                                 edm::ConsumesCollector& iC)
    : theHitProducer(iC.consumes<SiPixelRecHitCollection>(hitProducer)),
      theTtopo(iC.esConsumes()),
      theSide(side),
      theIdLayer(idLayer) {}

void HitExtractorPIX::useSkipClusters_(const edm::InputTag& m, edm::ConsumesCollector& iC) {
  theSkipClusters = iC.consumes<SkipClustersCollection>(m);
}

HitExtractor::Hits HitExtractorPIX::hits(const TkTransientTrackingRecHitBuilder& ttrhBuilder,
                                         const edm::Event& ev,
                                         const edm::EventSetup& es) const {
  HitExtractor::Hits result;

  const TrackerTopology& ttopo = es.getData(theTtopo);

  edm::Handle<SiPixelRecHitCollection> pixelHits;
  ev.getByToken(theHitProducer, pixelHits);
  if (theSide == TrackerDetSide::Barrel) {
    range2SeedingHits(*pixelHits, result, ttopo.pxbDetIdLayerComparator(theIdLayer));
  } else {
    range2SeedingHits(*pixelHits, result, ttopo.pxfDetIdDiskComparator(static_cast<unsigned int>(theSide), theIdLayer));
  }

  if (skipClusters) {
    LogDebug("HitExtractorPIX") << "getting : " << result.size() << " pixel hits.";
    //std::cout<<" skipping"<<std::endl;
    edm::Handle<SkipClustersCollection> pixelClusterMask;
    ev.getByToken(theSkipClusters, pixelClusterMask);
    unsigned int skipped = 0;
    for (unsigned int iH = 0; iH != result.size(); ++iH) {
      if (result[iH]->isValid()) {  // can be NOT valid???
        auto const& concrete = (SiPixelRecHit const&)(*result[iH]);
        assert(pixelClusterMask->refProd().id() == concrete.cluster().id());
        if (pixelClusterMask->mask(concrete.cluster().key())) {
          //too much debug LogDebug("HitExtractorPIX")<<"skipping a pixel hit on: "<< result[iH]->hit()->geographicalId().rawId()<<" key: "<<find(f->begin(),f->end(),concrete->cluster())->key();
          skipped++;
          result[iH].reset();
        }
      }
    }
    LogDebug("HitExtractorPIX") << "skipped :" << skipped << " pixel clusters";
    // std::cout << "HitExtractorPIX " <<"skipped :"<<skipped<<" pixel clusters out of " << result.size() << std::endl;
    if (skipped > 0) {
      auto last = std::remove_if(result.begin(), result.end(), [](HitPointer const& p) { return p.empty(); });
      result.resize(last - result.begin());
    }
  }
  LogDebug("HitExtractorPIX") << "giving :" << result.size() << " rechits out";
  // std::cout << "HitExtractorPIX "<<"giving :"<<result.size()<<" rechits out" << std::endl;
  return result;
}
