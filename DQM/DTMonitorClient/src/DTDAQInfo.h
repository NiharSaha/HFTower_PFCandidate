#ifndef DTMonitorClient_DTDAQInfo_H
#define DTMonitorClient_DTDAQInfo_H

/** \class DTDAQInfo
 *  No description available.
 *
 *  \author G. Cerminara - INFN Torino
 *
 *  threadsafe version (//-) oct/nov 2014 - WATWanAbdullah ncpp-um-my
 *
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "DQMServices/Core/interface/DQMEDHarvester.h"

#include "CondFormats/DataRecord/interface/DTReadOutMappingRcd.h"
#include "CondFormats/DataRecord/interface/RunSummaryRcd.h"

#include <map>

class DTReadOutMapping;
class RunInfo;

class DTDAQInfo : public DQMEDHarvester {
public:
  /// Constructor
  DTDAQInfo(const edm::ParameterSet &pset);

  /// Destructor
  ~DTDAQInfo() override;

  // Operations

protected:
  void dqmEndLuminosityBlock(DQMStore::IBooker &,
                             DQMStore::IGetter &,
                             edm::LuminosityBlock const &,
                             edm::EventSetup const &) override;
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override;

private:
  bool bookingdone;

  // Check FEDs from uROS, otherwise standard ROS
  bool checkUros;

  MonitorElement *totalDAQFraction;
  MonitorElement *daqMap;
  std::map<int, MonitorElement *> daqFractions;
  edm::ESGetToken<DTReadOutMapping, DTReadOutMappingRcd> mappingToken_;
  const DTReadOutMapping *mapping;

  edm::ESGetToken<RunInfo, RunInfoRcd> runInfoToken_;
  const RunInfo *sumFED;
};

#endif
