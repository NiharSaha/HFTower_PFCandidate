#ifndef CocoaDBMgr_HH
#define CocoaDBMgr_HH
//-*- C++ -*-
//
// Package:    Alignment/CocoaFit
// Class:      CocoaDBMgr
//
/*

 Description: 
*/
//

#include <stdexcept>
#include <string>
#include <iostream>
#include <map>
#include <memory>

class Event;
class EventSetup;
class Entry;
class Alingments;
//#include "FWCore/Framework/interface/EventSetup.h"

class OpticalAlignments;
class OpticalAlignInfo;
class OpticalAlignMeasurements;
class Alignments;
class AlignmentErrorsExtended;
class AlignTransform;
class AlignTransformErrorExtended;

class OpticalObject;

class CocoaDBMgr {
private:
  CocoaDBMgr();

public:
  static CocoaDBMgr* getInstance();

  ~CocoaDBMgr() {}

public:
  bool DumpCocoaResults();

private:
  std::unique_ptr<OpticalAlignments> BuildOpticalAlignments();
  double GetEntryError(const Entry* entry);
  double GetEntryError(const Entry* entry1, const Entry* entry2);

  OpticalAlignInfo GetOptAlignInfoFromOptO(OpticalObject* opto);

  std::pair<std::unique_ptr<Alignments>, std::unique_ptr<AlignmentErrorsExtended> > BuildAlignments(bool bDT);
  AlignTransform* GetAlignInfoFromOptO(OpticalObject* opto);
  AlignTransformErrorExtended* GetAlignInfoErrorFromOptO(OpticalObject* opto);

private:
  static CocoaDBMgr* instance;
};

#endif
