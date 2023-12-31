// -*- C++ -*-
//
//
/**\class dEdxHitAnalyzer dEdxHitAnalyzer.cc 
Monitoring source for general quantities related to track dEdx.
*/
// Original Author: Loic Quertenmont 2012/07/25

#include <memory>
#include <fstream>

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/TrackReco/interface/DeDxHitInfo.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

class GenericTriggerEventFlag;

class dEdxHitAnalyzer : public DQMEDAnalyzer {
public:
  explicit dEdxHitAnalyzer(const edm::ParameterSet&);
  ~dEdxHitAnalyzer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  double harmonic2(const reco::DeDxHitInfo* dedxHits);
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void dqmBeginRun(const edm::Run&, const edm::EventSetup&) override;

private:
  // ----------member data ---------------------------
  edm::ParameterSet fullconf_;
  edm::ParameterSet conf_;

  bool doAllPlots_;
  bool doDeDxPlots_;

  struct dEdxMEs {
    MonitorElement* ME_StripHitDeDx;
    MonitorElement* ME_PixelHitDeDx;
    MonitorElement* ME_NHitDeDx;
    MonitorElement* ME_Harm2DeDx;

    dEdxMEs() : ME_StripHitDeDx(nullptr), ME_PixelHitDeDx(nullptr), ME_NHitDeDx(nullptr), ME_Harm2DeDx(nullptr) {}
  };

  edm::InputTag trackInputTag_;
  edm::EDGetTokenT<reco::TrackCollection> trackToken_;

  std::vector<std::string> dEdxInputList_;
  std::vector<edm::EDGetTokenT<reco::DeDxHitInfoAss> > dEdxTokenList_;

  std::string TrackName;
  std::vector<std::string> AlgoNames;
  std::vector<dEdxMEs> dEdxMEsVector;
  std::string histname;  //for naming the histograms according to algorithm used

  GenericTriggerEventFlag* genTriggerEventFlag_;

  std::string MEFolderName;

  int dEdxNHitBin;
  double dEdxNHitMin;
  double dEdxNHitMax;

  int dEdxStripBin;
  double dEdxStripMin;
  double dEdxStripMax;

  int dEdxPixelBin;
  double dEdxPixelMin;
  double dEdxPixelMax;

  int dEdxHarm2Bin;
  double dEdxHarm2Min;
  double dEdxHarm2Max;
};
