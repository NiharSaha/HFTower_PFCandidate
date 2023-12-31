#ifndef DPGAnalysis_Skim_TagProbeMassEDMFilter_h
#define DPGAnalysis_Skim_TagProbeMassEDMFilter_h
// -*- C++ -*-
//
// Package:     TagAndProbe
// Class  :     TagProbeMassEDMFilter
//
/**\class TagProbeMassEDMFilter TagProbeMassEDMFilter.h PhysicsTools/TagAndProbe/interface/TagProbeMassEDMFilter.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author: Nadia Adam (Princeton)
//         Created:  Fri Jun  6 09:13:10 CDT 2008
// $Id: TagProbeMassEDMFilter.h,v 1.1 2010/05/04 09:42:40 azzi Exp $
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class TagProbeMassEDMFilter : public edm::one::EDFilter<> {
public:
  explicit TagProbeMassEDMFilter(const edm::ParameterSet&);
  ~TagProbeMassEDMFilter() override;

private:
  void beginJob() override;
  bool filter(edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
  std::string tpMapName;
};
#endif
