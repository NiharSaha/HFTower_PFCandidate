import FWCore.ParameterSet.Config as cms

process = cms.Process("read")
process.load('CondCore.CondDB.CondDB_cfi')
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = "INFO"
process.MessageLogger.cerr.CaloAlignmentRcdWrite = dict()

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.source = cms.Source("EmptySource")

process.CondDB.timetype = cms.untracked.string('runnumber')
process.CondDB.connect = cms.string('sqlite_file:testEcalAlignments.db')
process.PoolDBOutputService = cms.Service("PoolDBOutputService",
    process.CondDB,
    toPut = cms.VPSet(
      cms.PSet(
        record = cms.string('EBAlignmentRcd'),
        tag = cms.string('EB')
      ),
      cms.PSet(
        record = cms.string('EEAlignmentRcd'),
        tag = cms.string('EE')
      ),
      cms.PSet(
        record = cms.string('ESAlignmentRcd'),
        tag = cms.string('ES')
      )
    )
)

process.load('Geometry.CaloEventSetup.TestCaloAlignments_cff')

process.CaloAlignmentRcdWrite = cms.EDAnalyzer("CaloAlignmentRcdWrite")

process.p = cms.Path(process.CaloAlignmentRcdWrite)
