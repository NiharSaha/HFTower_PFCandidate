#ifndef HLTMuonL1TFilter_h
#define HLTMuonL1TFilter_h

/* \class HLTMuonL1TFilter
 *
 * This is an HLTFilter implementing filtering on L1T Stage2 GMT objects
 * 
 * \author:  V. Rekovic
*/

// user include files

#include "HLTrigger/HLTcore/interface/HLTFilter.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

class HLTMuonL1TFilter : public HLTFilter {
public:
  explicit HLTMuonL1TFilter(const edm::ParameterSet&);
  ~HLTMuonL1TFilter() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  bool hltFilter(edm::Event&,
                 const edm::EventSetup&,
                 trigger::TriggerFilterObjectWithRefs& filterproduct) const override;

private:
  //input tag identifying the product containing muons
  edm::InputTag candTag_;
  edm::EDGetTokenT<l1t::MuonBxCollection> candToken_;

  /// input tag identifying the product containing refs to muons passing the previous level
  edm::InputTag previousCandTag_;
  edm::EDGetTokenT<trigger::TriggerFilterObjectWithRefs> previousCandToken_;

  /// max Eta cut
  const double maxEta_;

  /// pT threshold
  const double minPt_;

  /// max dRs for L1 candidate matching
  const double maxDR_;
  const double maxDR2_;

  /// Quality codes:
  /// to be updated with new L1 quality definitions
  int qualityBitMask_;

  /// min N objects
  const double minN_;

  /// use central bx only muons
  const bool centralBxOnly_;
};

#endif  //HLTMuonL1TFilter_h
