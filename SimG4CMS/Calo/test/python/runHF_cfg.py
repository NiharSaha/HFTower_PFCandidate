import FWCore.ParameterSet.Config as cms

process = cms.Process("PROD")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("IOMC.EventVertexGenerators.VtxSmearedGauss_cfi")
process.load("Geometry.CMSCommonData.cmsHFPMTFibreXML_cfi")
process.load("Geometry.TrackerNumberingBuilder.trackerNumberingGeometry_cff")
process.load("Geometry.EcalCommonData.ecalSimulationParameters_cff")
process.load("Geometry.HcalCommonData.hcalDDConstants_cff")
process.load("Geometry.MuonNumbering.muonGeometryConstants_cff")
process.load("Geometry.MuonNumbering.muonOffsetESProducer_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
process.load("Configuration.EventContent.EventContent_cff")
process.load("SimG4Core.Application.g4SimHits_cfi")
process.load("SimG4CMS.Calo.HFPMTHitAnalyzer_cfi")

if 'MessageLogger' in process.__dict__:
    process.MessageLogger.G4cerr=dict()
    process.MessageLogger.G4cout=dict()
    process.MessageLogger.HFShower=dict()

process.load("IOMC.RandomEngine.IOMC_cff")
process.RandomNumberGeneratorService.generator.initialSeed = 456789
process.RandomNumberGeneratorService.g4SimHits.initialSeed = 9876
process.RandomNumberGeneratorService.VtxSmeared.initialSeed = 123456789

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run1_mc', '')

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
        PartID = cms.vint32(13),
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

process.o1 = cms.OutputModule("PoolOutputModule",
    process.FEVTSIMEventContent,
    fileName = cms.untracked.string('simevent.root')
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('HFPMTFibre1.root')
)

process.common_maximum_timex = cms.PSet(
    MaxTrackTime            = cms.double(500.0),
    MaxTrackTimeForward     = cms.double(2000.0), # ns
    MaxTimeNames            = cms.vstring(),
    MaxTrackTimes           = cms.vdouble(),
    MaxZCentralCMS          = cms.double(50.0), # m
    DeadRegions             = cms.vstring(),
    CriticalEnergyForVacuum = cms.double(2.0),
    CriticalDensity         = cms.double(1e-15)
)

process.p1 = cms.Path(process.generator*process.VtxSmeared*process.g4SimHits*process.HFPMTHitAnalyzer)
process.outpath = cms.EndPath(process.o1)
process.g4SimHits.Physics.type = 'SimG4Core/Physics/QGSP_BERT_EML'
process.g4SimHits.Physics.DefaultCutValue   = 0.1
process.g4SimHits.HCalSD.UseShowerLibrary   = False
process.g4SimHits.HCalSD.UseParametrize     = True
process.g4SimHits.HCalSD.UsePMTHits         = True
process.g4SimHits.HCalSD.UseFibreBundleHits = True
process.g4SimHits.HFShower.UseShowerLibrary = False
process.g4SimHits.HFShower.UseHFGflash      = True
process.g4SimHits.HFShower.TrackEM          = False
process.g4SimHits.HFShower.HFShowerBlock.OnlyLong = cms.bool(True)
process.g4SimHits.HFShower.EminLibrary      = 0.0
process.g4SimHits.HFShower.ApplyFiducialCut = True
process.g4SimHits.StackingAction = cms.PSet(
    process.common_heavy_suppression,
    process.common_maximum_timex,
    KillDeltaRay  = cms.bool(False),
    TrackNeutrino = cms.bool(False),
    KillHeavy     = cms.bool(False),
    KillGamma     = cms.bool(True),
    GammaThreshold = cms.double(0.0001), ## (MeV)
    SaveFirstLevelSecondary = cms.untracked.bool(True),
    SavePrimaryDecayProductsAndConversionsInTracker = cms.untracked.bool(True),
    SavePrimaryDecayProductsAndConversionsInCalo    = cms.untracked.bool(True),
    SavePrimaryDecayProductsAndConversionsInMuon    = cms.untracked.bool(True),
    RusRoGammaEnergyLimit  = cms.double(5.0), ## (MeV)
    RusRoEcalGamma         = cms.double(0.3),
    RusRoHcalGamma         = cms.double(0.3),
    RusRoMuonIronGamma     = cms.double(0.3),
    RusRoPreShowerGamma    = cms.double(0.3),
    RusRoCastorGamma       = cms.double(0.3),
    RusRoWorldGamma        = cms.double(0.3),
    RusRoNeutronEnergyLimit  = cms.double(10.0), ## (MeV)
    RusRoEcalNeutron         = cms.double(0.1),
    RusRoHcalNeutron         = cms.double(0.1),
    RusRoMuonIronNeutron     = cms.double(0.1),
    RusRoPreShowerNeutron    = cms.double(0.1),
    RusRoCastorNeutron       = cms.double(0.1),
    RusRoWorldNeutron        = cms.double(0.1),
    RusRoProtonEnergyLimit  = cms.double(0.0),
    RusRoEcalProton         = cms.double(1.0),
    RusRoHcalProton         = cms.double(1.0),
    RusRoMuonIronProton     = cms.double(1.0),
    RusRoPreShowerProton    = cms.double(1.0),
    RusRoCastorProton       = cms.double(1.0),
    RusRoWorldProton        = cms.double(1.0)
)
process.g4SimHits.SteppingAction = cms.PSet(
    process.common_maximum_timex,
    MaxNumberOfSteps        = cms.int32(50000),
    EkinNames               = cms.vstring(),
    EkinThresholds          = cms.vdouble(),
    EkinParticles           = cms.vstring(),
    Verbosity               = cms.untracked.int32(2)
)
process.g4SimHits.Watchers = cms.VPSet(cms.PSet(
    CheckForHighEtPhotons = cms.untracked.bool(False),
    TrackMin     = cms.untracked.int32(0),
    TrackMax     = cms.untracked.int32(99999999),
    TrackStep    = cms.untracked.int32(1),
    EventMin     = cms.untracked.int32(0),
    EventMax     = cms.untracked.int32(0),
    EventStep    = cms.untracked.int32(1),
    PDGids       = cms.untracked.vint32(),
    VerboseLevel = cms.untracked.int32(0),
    G4Verbose    = cms.untracked.bool(True),
    DEBUG        = cms.untracked.bool(False),
    type         = cms.string('TrackingVerboseAction')
))

