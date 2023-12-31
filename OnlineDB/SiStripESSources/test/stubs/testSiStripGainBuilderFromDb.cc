#include "CalibFormats/SiStripObjects/interface/SiStripFecCabling.h"
#include "CondFormats/DataRecord/interface/SiStripApvGainRcd.h"
#include "CondFormats/SiStripObjects/interface/SiStripApvGain.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

/**
   @class testSiStripGainBuilderFromDb 
   @brief Analyzes FEC (and FED) cabling object(s)
*/
class testSiStripGainBuilderFromDb : public edm::one::EDAnalyzer<edm::one::WatchRuns> {
public:
  testSiStripGainBuilderFromDb(const edm::ParameterSet&) : gainToken_(esConsumes<edm::Transition::BeginRun>()) {}

  ~testSiStripGainBuilderFromDb() override = default;
  void beginRun(const edm::Run&, const edm::EventSetup&) override;
  void endRun(const edm::Run&, const edm::EventSetup&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

private:
  const edm::ESGetToken<SiStripApvGain, SiStripApvGainRcd> gainToken_;
};

void testSiStripGainBuilderFromDb::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {}

// -----------------------------------------------------------------------------
void testSiStripGainBuilderFromDb::endRun(const edm::Run& run, const edm::EventSetup& setup) {}

// -----------------------------------------------------------------------------
void testSiStripGainBuilderFromDb::beginRun(const edm::Run& run, const edm::EventSetup& setup) {
  const SiStripApvGain* gain = &setup.getData(gainToken_);
  std::vector<uint32_t> ids;
  gain->getDetIds(ids);

  uint16_t ndets = ids.size();
  uint32_t napvs = 0;
  uint16_t isset = 0;
  uint16_t unset = 0;
  std::map<uint16_t, uint16_t> apvs;

  std::stringstream ss;
  ss << "[testSiStripGainBuilderFromDb::" << __func__ << "]"
     << " Values: " << std::endl;
  std::vector<uint32_t>::const_iterator ii = ids.begin();
  std::vector<uint32_t>::const_iterator jj = ids.end();
  for (; ii != jj; ++ii) {
    SiStripApvGain::Range range = gain->getRange(*ii);
    if (range.first == range.second) {
      continue;
    }
    std::vector<float>::const_iterator iii = range.first;
    std::vector<float>::const_iterator jjj = range.second;
    uint16_t temp = 0;
    ss << " DetId: " << *ii << " Gain: ";
    for (; iii != jjj; ++iii) {
      ss << std::fixed << std::setprecision(2) << *iii << " ";
      napvs++;
      temp++;
      if (fabs(*iii - 0.8) < 1.e-6) {
        unset++;
      } else {
        isset++;
      }
    }
    apvs[temp]++;
    ss << std::endl;
  }
  LogTrace("TEST") << ss.str();

  std::stringstream sss;
  sss << "[testSiStripGainBuilderFromDb::" << __func__ << "]"
      << " Summary: " << std::endl
      << " Ndets             : " << ndets << std::endl
      << " Napvs (total)     : " << napvs << std::endl
      << " Npavs (breakdown) : ";
  std::map<uint16_t, uint16_t>::const_iterator iiii = apvs.begin();
  std::map<uint16_t, uint16_t>::const_iterator jjjj = apvs.end();
  for (; iiii != jjjj; ++iiii) {
    sss << iiii->first << "/" << iiii->second << " ";
  }
  sss << std::endl << " Is set (!=0.8)    : " << isset << std::endl << " Not set (=0.8)    : " << unset;
  edm::LogVerbatim("TEST") << sss.str();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(testSiStripGainBuilderFromDb);
