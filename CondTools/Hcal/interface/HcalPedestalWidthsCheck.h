#ifndef HcalPedestalWidthsCheck_h
#define HcalPedestalWidthsCheck_h

//
// R.Ofierzynski 9.12.2007
//
// Code to check pedestals for completeness and to compare to previous pedestals

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/HcalObjects/interface/HcalPedestals.h"
#include "CondFormats/HcalObjects/interface/HcalPedestalWidths.h"
#include "CondFormats/HcalObjects/interface/HcalGains.h"
#include "CondFormats/HcalObjects/interface/HcalGainWidths.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"

#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "CondFormats/DataRecord/interface/HcalPedestalsRcd.h"
#include "CondFormats/DataRecord/interface/HcalPedestalWidthsRcd.h"
#include "CondFormats/DataRecord/interface/HcalGainsRcd.h"
#include "CondFormats/DataRecord/interface/HcalGainWidthsRcd.h"
#include "CondFormats/DataRecord/interface/HcalElectronicsMapRcd.h"

#include "CalibCalorimetry/HcalAlgos/interface/HcalDbASCIIIO.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"

class HcalPedestalWidthsCheck : public edm::one::EDAnalyzer<> {
public:
  HcalPedestalWidthsCheck(edm::ParameterSet const& ps);

  ~HcalPedestalWidthsCheck() override;

  void analyze(const edm::Event& ev, const edm::EventSetup& es) override;

private:
  std::string outfile;
  std::string dumprefs;
  std::string dumpupdate;
  bool checkemapflag;
  bool validateflag;
  double epsilon;
  edm::ESGetToken<HcalPedestalWidths, HcalPedestalWidthsRcd> m_tok1;
  edm::ESGetToken<HcalPedestalWidths, HcalPedestalWidthsRcd> m_tok2;
  edm::ESGetToken<HcalElectronicsMap, HcalElectronicsMapRcd> m_tokmap;
  //  vecDetId getMissingDetIds(std::vector<HcalPedestalWidths> &);
};
#endif
