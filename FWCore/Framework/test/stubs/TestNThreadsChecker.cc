// -*- C++ -*-
//
// Package:    Framework
// Class:      TestNThreadsChecker
//
/**\class TestNThreadsChecker TestNThreadsChecker.cc FWCore/Framework/test/stubs/TestNThreadsChecker.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Chris Jones
//         Created:  Thu Jan 03 11:02:00 EST 2013
//
//

// system include files
#include <memory>
#include <atomic>
#include <unistd.h>
#include "oneapi/tbb/task_arena.h"

// user include files
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
#include "FWCore/ServiceRegistry/interface/SystemBounds.h"

#include "FWCore/Utilities/interface/Exception.h"

//
// class decleration
//

class TestNThreadsChecker {
public:
  explicit TestNThreadsChecker(const edm::ParameterSet&, edm::ActivityRegistry&);

private:
  // ----------member data ---------------------------
  unsigned int m_nExpectedThreads;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TestNThreadsChecker::TestNThreadsChecker(const edm::ParameterSet& iConfig, edm::ActivityRegistry& iReg)
    : m_nExpectedThreads(iConfig.getUntrackedParameter<unsigned int>("nExpectedThreads")) {
  unsigned int expectedThreads = m_nExpectedThreads;
  if (expectedThreads == 0) {
    expectedThreads = oneapi::tbb::this_task_arena::max_concurrency();
  }

  //now do what ever initialization is needed
  iReg.watchPreallocate([expectedThreads](edm::service::SystemBounds const& iBounds) {
    if (expectedThreads != iBounds.maxNumberOfThreads()) {
      throw cms::Exception("UnexpectedNumberOfThreads")
          << "Expected " << expectedThreads << " threads but actual value is " << iBounds.maxNumberOfThreads();
    }
  });
}

//define this as a plug-in
DEFINE_FWK_SERVICE(TestNThreadsChecker);
