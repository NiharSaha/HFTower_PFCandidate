/** \class HLTPrescaleRecorder
 *
 * See header file for documentation
 *
 *
 *  \author Martin Grunewald
 *
 */

#include "HLTrigger/HLTcore/interface/HLTPrescaleRecorder.h"
#include "DataFormats/Provenance/interface/ProcessHistory.h"
#include "DataFormats/Provenance/interface/Timestamp.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include <sys/time.h>
#include <string>
#include <ostream>

using namespace std;
using namespace edm;
using namespace trigger;

//
// constructors and destructor
//
HLTPrescaleRecorder::HLTPrescaleRecorder(const edm::ParameterSet& ps)
    : src_(ps.getParameter<int>("src")),
      run_(ps.getParameter<bool>("run")),
      lumi_(ps.getParameter<bool>("lumi")),
      event_(ps.getParameter<bool>("event")),
      condDB_(ps.getParameter<bool>("condDB")),
      psetName_(ps.getParameter<string>("psetName")),
      hltInputTag_(ps.getParameter<InputTag>("hltInputTag")),
      hltInputToken_(),
      hltDBTag_(ps.getParameter<string>("hltDBTag")),
      hltPrescaleTableCondToken_(esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", hltDBTag_))),
      ps_(nullptr),
      db_(nullptr),
      hltHandle_(),
      hlt_() {
  if (src_ == 1) {
    // Run
    hltInputToken_ = consumes<trigger::HLTPrescaleTable, edm::InRun>(hltInputTag_);
  } else if (src_ == 2) {
    // Lumi
    hltInputToken_ = consumes<trigger::HLTPrescaleTable, edm::InLumi>(hltInputTag_);
  } else if (src_ == 3) {
    // Event
    hltInputToken_ = consumes<trigger::HLTPrescaleTable>(hltInputTag_);
  }

  LogInfo("HLTPrescaleRecorder") << "src:run-lumi-event-condDB+psetName+tags: " << src_ << ":" << run_ << "-" << lumi_
                                 << "-" << event_ << "-" << condDB_ << "+" << psetName_ << "+" << hltInputTag_.encode()
                                 << "+" << hltDBTag_;

  if (edm::Service<edm::service::PrescaleService>().isAvailable()) {
    ps_ = edm::Service<edm::service::PrescaleService>().operator->();
  } else if (src_ == 0) {
    LogError("HLTPrescaleRecorder") << "PrescaleService requested as source but unavailable!";
  }

  if (edm::Service<cond::service::PoolDBOutputService>().isAvailable()) {
    db_ = edm::Service<cond::service::PoolDBOutputService>().operator->();
  } else if (condDB_) {
    LogError("HLTPrescaleRecorder") << "PoolDBOutputService requested as destination but unavailable!";
  }

  if (run_)
    produces<HLTPrescaleTable, edm::Transition::EndRun>("Run");
  if (lumi_)
    produces<HLTPrescaleTable, edm::Transition::EndLuminosityBlock>("Lumi");
  if (event_)
    produces<HLTPrescaleTable, edm::Transition::Event>("Event");
}

HLTPrescaleRecorder::~HLTPrescaleRecorder() = default;

//
// member functions
//

void HLTPrescaleRecorder::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  //  # (single) source:
  //  # -1:PrescaleServicePSet, 0:PrescaleService,
  //  #  1:Run, 2:Lumi, 3:Event, 4:CondDB
  desc.add<int>("src", 0);
  //  # (multiple) destinations
  desc.add<bool>("run", true);
  desc.add<bool>("lumi", true);
  desc.add<bool>("event", true);
  desc.add<bool>("condDB", true);
  //  # src=-1
  desc.add<std::string>("psetName", "");
  //  # src= 1,2,3
  desc.add<edm::InputTag>("hltInputTag", edm::InputTag("", "", ""));
  //  # src= 4
  desc.add<std::string>("hltDBTag", "");
  //
  descriptions.add("hltPrescaleRecorder", desc);
}

void HLTPrescaleRecorder::beginRun(edm::Run const& iRun, const edm::EventSetup& iSetup) {
  hlt_ = HLTPrescaleTable();

  if (src_ == -1) {
    /// From PrescaleTable tracked PSet
    ParameterSet const& pPSet(getProcessParameterSetContainingModule(moduleDescription()));
    ParameterSet iPS(pPSet.getParameter<ParameterSet>(psetName_));

    string defaultLabel(iPS.getParameter<std::string>("lvl1DefaultLabel"));
    vector<string> labels(iPS.getParameter<std::vector<std::string> >("lvl1Labels"));
    vector<ParameterSet> vpTable(iPS.getParameter<std::vector<ParameterSet> >("prescaleTable"));

    unsigned int set(0);
    const unsigned int n(labels.size());
    for (unsigned int i = 0; i != n; ++i) {
      if (labels[i] == defaultLabel)
        set = i;
    }

    map<string, vector<unsigned int> > table;
    const unsigned int m(vpTable.size());
    for (unsigned int i = 0; i != m; ++i) {
      table[vpTable[i].getParameter<std::string>("pathName")] =
          vpTable[i].getParameter<std::vector<unsigned int> >("prescales");
    }
    hlt_ = HLTPrescaleTable(set, labels, table);

  } else if (src_ == 0) {
    /// From PrescaleService
    /// default index updated at lumi block boundaries
    if (ps_ != nullptr) {
      hlt_ = HLTPrescaleTable(ps_->getLvl1IndexDefault(), ps_->getLvl1Labels(), ps_->getPrescaleTable());
    } else {
      hlt_ = HLTPrescaleTable();
      LogError("HLTPrescaleRecorder") << "PrescaleService not found!";
    }
  } else if (src_ == 1) {
    /// From Run Block
    if (iRun.getByToken(hltInputToken_, hltHandle_)) {
      hlt_ = *hltHandle_;
    } else {
      LogError("HLTPrescaleRecorder") << "HLTPrescaleTable not found in Run!";
    }
  } else if (src_ == 4) {
    /// From CondDB (needs ESProducer module as well)
    auto const& hltPrescaleTableCond = iSetup.getData(hltPrescaleTableCondToken_);
    hlt_ = hltPrescaleTableCond.hltPrescaleTable();
  }

  return;
}

void HLTPrescaleRecorder::beginLuminosityBlock(edm::LuminosityBlock const& iLumi, const edm::EventSetup& iSetup) {
  if (src_ == 0) {
    /// From PrescaleService
    /// default index updated at lumi block boundaries
    if (ps_ != nullptr) {
      hlt_ = HLTPrescaleTable(ps_->getLvl1IndexDefault(), ps_->getLvl1Labels(), ps_->getPrescaleTable());
    } else {
      hlt_ = HLTPrescaleTable();
      LogError("HLTPrescaleRecorder") << "PrescaleService not found!";
    }
  } else if (src_ == 2) {
    /// From Lumi Block
    if (iLumi.getByToken(hltInputToken_, hltHandle_)) {
      hlt_ = *hltHandle_;
    } else {
      hlt_ = HLTPrescaleTable();
      LogError("HLTPrescaleRecorder") << "HLTPrescaleTable not found in LumiBlock!";
    }
  }

  return;
}

void HLTPrescaleRecorder::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if (src_ == 3) {
    /// From Event Block
    if (iEvent.getByToken(hltInputToken_, hltHandle_)) {
      hlt_ = *hltHandle_;
    } else {
      hlt_ = HLTPrescaleTable();
      LogError("HLTPrescaleRecorder") << "HLTPrescaleTable not found in Event!";
    }
  }

  if (event_) {
    /// Writing to Event
    unique_ptr<HLTPrescaleTable> product(new HLTPrescaleTable(hlt_));
    iEvent.put(std::move(product), "Event");
  }

  return;
}
void HLTPrescaleRecorder::endLuminosityBlock(edm::LuminosityBlock const& iLumi, const edm::EventSetup& iSetup) {}

void HLTPrescaleRecorder::endLuminosityBlockProduce(edm::LuminosityBlock& iLumi, const edm::EventSetup& iSetup) {
  if (lumi_) {
    /// Writing to Lumi Block
    unique_ptr<HLTPrescaleTable> product(new HLTPrescaleTable(hlt_));
    iLumi.put(std::move(product), "Lumi");
  }
  return;
}

void HLTPrescaleRecorder::endRun(edm::Run const& iRun, const edm::EventSetup& iSetup) {
  /// Dump to logfile
  ostringstream oss;
  const unsigned int n(hlt_.size());
  oss << "PrescaleTable: # of labels = " << n << endl;
  const vector<string>& labels(hlt_.labels());
  for (unsigned int i = 0; i != n; ++i) {
    oss << " " << i << "/'" << labels.at(i) << "'";
  }
  oss << endl;
  const map<string, vector<unsigned int> >& table(hlt_.table());
  oss << "PrescaleTable: # of paths = " << table.size() << endl;
  const map<string, vector<unsigned int> >::const_iterator tb(table.begin());
  const map<string, vector<unsigned int> >::const_iterator te(table.end());
  for (map<string, vector<unsigned int> >::const_iterator ti = tb; ti != te; ++ti) {
    for (unsigned int i = 0; i != n; ++i) {
      oss << " " << ti->second.at(i);
    }
    oss << " " << ti->first << endl;
  }
  LogVerbatim("HLTPrescaleRecorder") << oss.str();

  if (condDB_) {
    /// Writing to CondDB (needs PoolDBOutputService)
    if (db_ != nullptr) {
      HLTPrescaleTableCond product(hlt_);
      ::timeval tv;
      gettimeofday(&tv, nullptr);
      edm::Timestamp tstamp((unsigned long long)tv.tv_sec);
      db_->writeOneIOV(product, tstamp.value(), "HLTPrescaleTableRcd");
    } else {
      LogError("HLTPrescaleRecorder") << "PoolDBOutputService not available!";
    }
  }

  return;
}

void HLTPrescaleRecorder::endRunProduce(edm::Run& iRun, const edm::EventSetup& iSetup) {
  if (run_) {
    /// Writing to Run Block
    unique_ptr<HLTPrescaleTable> product(new HLTPrescaleTable(hlt_));
    iRun.put(std::move(product), "Run");
  }
}
