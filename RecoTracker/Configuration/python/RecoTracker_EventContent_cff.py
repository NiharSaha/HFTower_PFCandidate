# The following comments couldn't be translated into the new config version:

import FWCore.ParameterSet.Config as cms

#Full Event content 
RecoTrackerFEVT = cms.PSet(
    outputCommands = cms.untracked.vstring('keep recoTracks_generalTracks_*_*', 
        'keep recoTrackExtras_generalTracks_*_*', 
        'keep TrackingRecHitsOwned_generalTracks_*_*', 
        'keep recoTracks_rsWithMaterialTracks_*_*', 
        'keep recoTrackExtras_rsWithMaterialTracks_*_*', 
        'keep TrackingRecHitsOwned_rsWithMaterialTracks_*_*',
        'keep *_dedxTruncated40_*_*',
        'keep *_dedxMedian_*_*',
        'keep *_dedxHarmonic2_*_*',
    )
)
#RECO content
RecoTrackerRECO = cms.PSet(
    outputCommands = cms.untracked.vstring('keep recoTracks_generalTracks_*_*', 
        'keep recoTrackExtras_generalTracks_*_*', 
        'keep TrackingRecHitsOwned_generalTracks_*_*', 
        'keep recoTracks_rsWithMaterialTracks_*_*', 
        'keep recoTrackExtras_rsWithMaterialTracks_*_*', 
        'keep TrackingRecHitsOwned_rsWithMaterialTracks_*_*',
        'keep *_dedxTruncated40_*_*',
        'keep *_dedxMedian_*_*',
        'keep *_dedxHarmonic2_*_*',
    )
)
#AOD content
RecoTrackerAOD = cms.PSet(
    outputCommands = cms.untracked.vstring('keep recoTracks_generalTracks_*_*', 
        'keep recoTracks_rsWithMaterialTracks_*_*',
        'keep *_dedxTruncated40_*_*',
        'keep *_dedxMedian_*_*',
        'keep *_dedxHarmonic2_*_*',
    )
)

