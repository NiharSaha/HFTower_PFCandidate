import FWCore.ParameterSet.Config as cms

from DQM.SiStripMonitorSummary.OnDemandMonitoring_cfi import *
#  SiStripMonitorAnalyser ####
SiStripAnalyserCosmic = cms.EDAnalyzer("SiStripAnalyser",
    StaticUpdateFrequency    = cms.untracked.int32(1),
    GlobalStatusFilling      = cms.untracked.int32(2),
    TkMapCreationFrequency   = cms.untracked.int32(3),
    SummaryCreationFrequency = cms.untracked.int32(5),
    ShiftReportFrequency     = cms.untracked.int32(-1),
    SummaryConfigPath        = cms.untracked.string("DQM/SiStripMonitorClient/data/sistrip_monitorelement_config.xml"),
    PrintFaultyModuleList    = cms.untracked.bool(True),                                
    RawDataTag               = cms.untracked.InputTag("source"),                              
    TkmapParameters = cms.PSet(
        loadFedCabling = cms.untracked.bool(True),
        loadFecCabling = cms.untracked.bool(True),
        loadLVCabling  = cms.untracked.bool(True),
        loadHVCabling  = cms.untracked.bool(True),
        trackerdatPath = cms.untracked.string('CommonTools/TrackerMap/data/'),
        trackermaptxtPath = cms.untracked.string('DQM/Integration/test/TkMap/')
    ),

# Parameters for On Demand Monitoring   
    MonitorSiStrip_PSet          = OnDemandMonitoring.MonitorSiStrip_PSet,

    FillConditions_PSet          = OnDemandMonitoring.FillConditions_PSet,     

    SiStripPedestalsDQM_PSet     = OnDemandMonitoring.SiStripPedestalsDQM_PSet,
    SiStripNoisesDQM_PSet        = OnDemandMonitoring.SiStripNoisesDQM_PSet,
    SiStripQualityDQM_PSet       = OnDemandMonitoring.SiStripQualityDQM_PSet,
    SiStripApvGainsDQM_PSet      = OnDemandMonitoring.SiStripApvGainsDQM_PSet,
    SiStripLorentzAngleDQM_PSet  = OnDemandMonitoring.SiStripLorentzAngleDQM_PSet,
    SiStripLowThresholdDQM_PSet  = OnDemandMonitoring.SiStripLowThresholdDQM_PSet,
    SiStripHighThresholdDQM_PSet = OnDemandMonitoring.SiStripHighThresholdDQM_PSet,
)
# Track Efficiency Client

# Services needed for TkHistoMap
from CalibTracker.SiStripCommon.TkDetMapESProducer_cfi import *
