import FWCore.ParameterSet.Config as cms

process = cms.Process("PROD")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("IOMC.EventVertexGenerators.VtxSmearedGauss_cfi")
process.load("Geometry.CMSCommonData.cmsExtendedGeometryHFLibraryXML_cfi")
process.load("Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cff")
process.load("Geometry.EcalCommonData.ecalSimulationParameters_cff")
process.load("Geometry.HcalCommonData.hcalDDConstants_cff")
process.load("Geometry.MuonNumbering.muonGeometryConstants_cff")
process.load("Geometry.MuonNumbering.muonOffsetESProducer_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.Generator_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load("SimG4CMS.Calo.HFPMTHitAnalyzer_cfi")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff"
)
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['run1_mc']

if 'MessageLogger' in process.__dict__:
    process.MessageLogger.G4cerr=dict()
    process.MessageLogger.G4cout=dict()
    process.MessageLogger.HFShower=dict()

process.load("IOMC.RandomEngine.IOMC_cff")
process.RandomNumberGeneratorService.generator.initialSeed = 456789
process.RandomNumberGeneratorService.g4SimHits.initialSeed = 9876
process.RandomNumberGeneratorService.VtxSmeared.initialSeed = 123456789

process.Timing = cms.Service("Timing")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(200)
)

process.source = cms.Source("EmptySource",
    firstRun        = cms.untracked.uint32(1),
    firstEvent      = cms.untracked.uint32(1)
)

process.generator = cms.EDProducer("FlatRandomEGunProducer",
    PGunParameters = cms.PSet(
        PartID = cms.vint32(211),
        MinEta = cms.double(3.25),
        MaxEta = cms.double(3.50),
        MinPhi = cms.double(-3.1415926),
        MaxPhi = cms.double(3.1415926),
        MinE   = cms.double(1000.00),
        MaxE   = cms.double(1000.00)
    ),
    Verbosity       = cms.untracked.int32(0),
    AddAntiParticle = cms.bool(False)
)

process.output = cms.OutputModule("PoolOutputModule",
    process.FEVTSIMEventContent,
    fileName = cms.untracked.string('simevent.root')
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('HFShower.root')
)

process.common_maximum_timex = cms.PSet(
    MaxTrackTime  = cms.double(500.0),
    MaxTimeNames  = cms.vstring(),
    MaxTrackTimes = cms.vdouble()
)

process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.analysis_step   = cms.Path(process.HFPMTHitAnalyzer)
process.out_step = cms.EndPath(process.output)

process.g4SimHits.Physics.type = 'SimG4Core/Physics/QGSP_FTFP_BERT_EML'
process.g4SimHits.Physics.DefaultCutValue   = 0.1
process.g4SimHits.HCalSD.UseShowerLibrary   = False
process.g4SimHits.HCalSD.UseParametrize     = False
process.g4SimHits.HCalSD.UsePMTHits         = False
process.g4SimHits.HCalSD.UseFibreBundleHits = False
process.g4SimHits.HFShower.UseShowerLibrary = False
process.g4SimHits.HFShower.UseHFGflash      = False
process.g4SimHits.HFShower.TrackEM          = False
process.g4SimHits.HFShower.HFShowerBlock.OnlyLong = cms.bool(False)
process.g4SimHits.HFShower.EminLibrary      = 0.0

# Schedule definition                                                          
process.schedule = cms.Schedule(process.generation_step,
                                process.simulation_step,
                                process.analysis_step,
                                process.out_step
                                )

# filter all path with the production filter sequence                          
for path in process.paths:
        getattr(process,path)._seq = process.generator * getattr(process,path)._seq

