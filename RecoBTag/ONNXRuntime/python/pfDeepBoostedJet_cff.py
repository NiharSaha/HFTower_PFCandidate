import FWCore.ParameterSet.Config as cms

from RecoBTag.FeatureTools.pfDeepBoostedJetTagInfos_cfi import pfDeepBoostedJetTagInfos
from RecoBTag.ONNXRuntime.boostedJetONNXJetTagsProducer_cfi import boostedJetONNXJetTagsProducer
from RecoBTag.ONNXRuntime.Parameters.DeepBoostedJet.V02.pfDeepBoostedJetPreprocessParams_cfi import pfDeepBoostedJetPreprocessParams
from RecoBTag.ONNXRuntime.Parameters.DeepBoostedJet.V02.pfMassDecorrelatedDeepBoostedJetPreprocessParams_cfi import pfMassDecorrelatedDeepBoostedJetPreprocessParams
from RecoBTag.ONNXRuntime.pfDeepBoostedDiscriminatorsJetTags_cfi import pfDeepBoostedDiscriminatorsJetTags
from RecoBTag.ONNXRuntime.pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags_cfi import pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags

# nominal DeepAK8
pfDeepBoostedJetTags = boostedJetONNXJetTagsProducer.clone(
    preprocessParams = pfDeepBoostedJetPreprocessParams,
    model_path = 'RecoBTag/Combined/data/DeepBoostedJet/V02/full/resnet.onnx',
)

# mass-decorrelated DeepAK8
pfMassDecorrelatedDeepBoostedJetTags = boostedJetONNXJetTagsProducer.clone(
    preprocessParams = pfMassDecorrelatedDeepBoostedJetPreprocessParams,
    model_path = 'RecoBTag/Combined/data/DeepBoostedJet/V02/decorrelated/resnet.onnx',
)

from CommonTools.PileupAlgos.Puppi_cff import puppi
from CommonTools.RecoAlgos.primaryVertexAssociation_cfi import primaryVertexAssociation

# This task is not used, useful only if we run DeepFlavour from RECO
# jets (RECO/AOD)
pfDeepBoostedJetTask = cms.Task(puppi, primaryVertexAssociation,
                             pfDeepBoostedJetTagInfos, pfDeepBoostedJetTags, pfMassDecorrelatedDeepBoostedJetTags,
                             pfDeepBoostedDiscriminatorsJetTags, pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags)

# declare all the discriminators
# nominal: probs
_pfDeepBoostedJetTagsProbs = ['pfDeepBoostedJetTags:' + flav_name
                              for flav_name in pfDeepBoostedJetTags.flav_names]
# nominal: meta-taggers
_pfDeepBoostedJetTagsMetaDiscrs = ['pfDeepBoostedDiscriminatorsJetTags:' + disc.name.value()
                                   for disc in pfDeepBoostedDiscriminatorsJetTags.discriminators]

# mass-decorrelated: probs
_pfMassDecorrelatedDeepBoostedJetTagsProbs = ['pfMassDecorrelatedDeepBoostedJetTags:' + flav_name
                                              for flav_name in pfMassDecorrelatedDeepBoostedJetTags.flav_names]
# mass-decorrelated: meta-taggers
_pfMassDecorrelatedDeepBoostedJetTagsMetaDiscrs = ['pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:' + disc.name.value()
                                   for disc in pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags.discriminators]

_pfDeepBoostedJetTagsAll = _pfDeepBoostedJetTagsProbs + _pfDeepBoostedJetTagsMetaDiscrs + \
    _pfMassDecorrelatedDeepBoostedJetTagsProbs + _pfMassDecorrelatedDeepBoostedJetTagsMetaDiscrs
