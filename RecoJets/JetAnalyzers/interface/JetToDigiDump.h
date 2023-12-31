#ifndef RecoExamples_JetToDigiDump_h
#define RecoExamples_JetToDigiDump_h
#include <TH1.h>
#include <TProfile.h>
#include <TH2.h>
/* \class JetToDigiDump
 *
 * \author Robert Harris
 *
 * \version 1
 *
 */
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

class JetToDigiDump : public edm::one::EDAnalyzer<> {
public:
  JetToDigiDump(const edm::ParameterSet&);

private:
  //Framwework stuff
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // Parameters passed via the config file
  std::string DumpLevel;   //How deep into calorimeter reco to dump
  std::string CaloJetAlg;  //Jet Algorithm to dump
  int DebugLevel;          //0 = no debug prints
  bool ShowECal;           //if true, ECAL hits are ignored

  //Internal parameters
  int Dump;
  int evtCount;
};

#endif
