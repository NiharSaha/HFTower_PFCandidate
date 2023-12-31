#ifndef special_HLTDTROMonitorFilter_H
#define special_HLTDTROMonitorFilter_H

/** \class HLTDTROMonitorFilter.h
 *  No description available.
 *
 *  \author G. Cerminara - INFN Torino
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/global/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

namespace edm {
  class ConfigurationDescriptions;
}

class HLTDTROMonitorFilter : public edm::global::EDFilter<> {
public:
  /// Constructor
  HLTDTROMonitorFilter(const edm::ParameterSet&);

  /// Destructor
  ~HLTDTROMonitorFilter() override;

  // Operations
  bool filter(edm::StreamID, edm::Event& event, const edm::EventSetup& setup) const override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::EDGetTokenT<FEDRawDataCollection> inputToken;
};
#endif
