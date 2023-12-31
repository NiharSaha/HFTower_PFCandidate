import FWCore.ParameterSet.Config as cms

#
# generator level info
#
from PhysicsTools.HepMCCandAlgos.genParticles_cfi import *
from GeneratorInterface.Core.generatorSmeared_cfi import *
from RecoJets.Configuration.RecoGenJets_cff import *
from RecoMET.Configuration.RecoGenMET_cff import *
from RecoJets.Configuration.GenJetParticles_cff import *
from RecoMET.Configuration.GenMETParticles_cff import *
# Vertex smearing
#
# There're several types of Vertex smearing module (Gauss, Flat, BeamProfile, Betafunc, BetafuncEarlyCollisions);
# the cff's are available for each one :
# Configuration/StandardSequences/data/VtxSmearedGauss.cff
# Configuration/StandardSequences/data/VtxSmearedFlat.cff
# Configuration/StandardSequences/data/VtxSmearedBeamProfile.cff
# Configuration/StandardSequences/data/VtxSmearedBetafuncNominalCollision.cff
# Configuration/StandardSequences/data/VtxSmearedBetafuncEarlyCollision.cff
#
# Either of the above returns label "generatorSmeared" that is already in the path below
# (but not included here!!!)
#   
# Note 1 : one and only label is allowed for either of these modules,
#          that is VtxSmeared (the reason is not to allow multiple smearing); 
#          an attempt to use any other label will cause the Fwk to throw
# Note 2 : because only one label is allowed, a user can have only one
#          instance of the Vertex smearing module in a given configuration;
#          in other words, if you put in 2 or all 3 of the available cfi's, 
#          the Fwk will throw
#
# Currently, we suggest using Betafunc vertex smearing in the desired LHC configuration 
#
# The vertex smearing needs to be called before the particle candidate generation
#
# REMINDER : vertex smearing need a startup seed;
# in your cfg, do NOT forget to give seeds via RandomNumberGeneratorService !!!
# 
# Example configuration of the RandomNumberGeneratorService to appear in cfg:
# service = RandomNumberGeneratorService
# {
#   untracked uint32 sourceSeed = 123456789
#   PSet moduleSeeds =
#   {
#      untracked uint32 VtxSmeared = 98765432
#   }
# }

GeneInfoTask = cms.Task(genParticles)
genJetMETTask = cms.Task(genJetParticlesTask, recoGenJetsTask, genMETParticlesTask, recoGenMETTask)

VertexSmearing = cms.Sequence(cms.SequencePlaceholder("VtxSmeared"))
GenSmeared = cms.Sequence(generatorSmeared)
GeneInfo = cms.Sequence(GeneInfoTask)
genJetMET = cms.Sequence(genJetMETTask)

from SimPPS.Configuration.GenPPS_cff import *
pgen = cms.Sequence(cms.SequencePlaceholder("randomEngineStateProducer")+VertexSmearing+GenSmeared+GeneInfo+genJetMET, PPSTransportTask)

# sequence for bare generator result only, without vertex smearing and analysis objects added
pgen_genonly = cms.Sequence(cms.SequencePlaceholder("randomEngineStateProducer"))

# only vertex smearing and analysis objects
pgen_smear = cms.Sequence(VertexSmearing+GenSmeared+GeneInfo+genJetMET, PPSTransportTask)

fixGenInfoTask = cms.Task(GeneInfoTask, genJetMETTask)
fixGenInfo = cms.Sequence(fixGenInfoTask)


import HLTrigger.HLTfilters.triggerResultsFilter_cfi
genstepfilter = HLTrigger.HLTfilters.triggerResultsFilter_cfi.triggerResultsFilter.clone(
    l1tResults = cms.InputTag(''),
    hltResults = cms.InputTag('TriggerResults'),
    triggerConditions = cms.vstring()
)
