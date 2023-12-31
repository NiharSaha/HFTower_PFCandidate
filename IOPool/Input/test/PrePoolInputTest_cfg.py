# The following comments couldn't be translated into the new config version:

# Configuration file for PrePoolInputTest 

import FWCore.ParameterSet.Config as cms
import sys

argv = []
foundpy = False
for a in sys.argv:
    if foundpy:
        argv.append(a)
    if ".py" in a:
        foundpy = True

useOtherThing = False
if len(argv) > 6:
  if argv[6] == "useOtherThing":
    useOtherThing = True

process = cms.Process("TESTPROD")
process.load("FWCore.Framework.test.cmsExceptionsFatal_cff")

process.maxEvents.input = int(argv[1])

process.Thing = cms.EDProducer("ThingProducer")

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string(argv[0])
)

process.source = cms.Source("EmptySource",
    firstRun = cms.untracked.uint32(int(argv[2])),
    numberEventsInRun = cms.untracked.uint32(int(argv[3])),
    firstLuminosityBlock = cms.untracked.uint32(int(argv[4])),
    numberEventsInLuminosityBlock = cms.untracked.uint32(int(argv[5]))
)

process.p = cms.Path(process.Thing)
if useOtherThing:
  process.OtherThing = cms.EDProducer("OtherThingProducer")
  process.p = cms.Path(process.Thing + process.OtherThing)
process.ep = cms.EndPath(process.output)


