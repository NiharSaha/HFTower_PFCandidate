import FWCore.ParameterSet.Config as cms

from ..modules.filteredLayerClustersTrkEM_cfi import *
from ..modules.ticlSeedingTrk_cfi import *
from ..modules.ticlTrackstersTrkEM_cfi import *

ticlTrkEMStepTask = cms.Task(filteredLayerClustersTrkEM, ticlSeedingTrk, ticlTrackstersTrkEM)
