// -*- C++ -*-
//
// Package:     HcalTestBeam
// Class  :     HcalTB02XtalNumberingScheme
//
// Implementation:
//     Numbering scheme for crystal calorimeter in 2002 test beam
//
// Original Author:
//         Created:  Sun 21 10:14:34 CEST 2006
//

// system include files

// user include files
#include "SimG4CMS/HcalTestBeam/interface/HcalTB02XtalNumberingScheme.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//#define EDM_ML_DEBUG
//
// constructors and destructor
//

HcalTB02XtalNumberingScheme::HcalTB02XtalNumberingScheme() : HcalTB02NumberingScheme() {
  edm::LogVerbatim("HcalTBSim") << "Creating HcalTB02XtalNumberingScheme";
}

HcalTB02XtalNumberingScheme::~HcalTB02XtalNumberingScheme() {
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HcalTBSim") << "Deleting HcalTB02XtalNumberingScheme";
#endif
}

//
// member functions
//

int HcalTB02XtalNumberingScheme::getUnitID(const G4Step* aStep) const {
  const G4VTouchable* touch = aStep->GetPreStepPoint()->GetTouchable();
  int idx = touch->GetReplicaNumber(0);
  int idl = 0;
  if (touch->GetHistoryDepth() > 0)
    idl = touch->GetReplicaNumber(1);
  int idunit = idl * 100 + idx;
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("HcalTBSim") << "HcalTB02XtalNumberingScheme:: Row " << idl << " Column " << idl
                                << " idunit = " << idunit;
#endif
  return idunit;
}
