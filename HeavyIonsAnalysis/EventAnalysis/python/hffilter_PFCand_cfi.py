import FWCore.ParameterSet.Config as cms

# select HF candidate energy
hfPFCandidates = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("packedPFCandidates"),
    cut = cms.string("(pdgId() == 1 || pdgId() == 2) && (abs(eta()) >= 3.0 && abs(eta()) <= 6.0)")
)

towersAboveThreshold = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("hfPFCandidates"),
    cut = cms.string("energy() >= 3")
)

# make calotowers into candidates with threshold 4
towersAboveThresholdTh2 = towersAboveThreshold.clone(cut = cms.string("energy() >= 2.0"))
towersAboveThresholdTh3 = towersAboveThreshold.clone(cut = cms.string("energy() >= 3.0")) #Added
towersAboveThresholdTh4 = towersAboveThreshold.clone(cut = cms.string("energy() >= 4.0"))
towersAboveThresholdTh5 = towersAboveThreshold.clone(cut = cms.string("energy() >= 5.0"))
towersAboveThresholdTh6 = towersAboveThreshold.clone(cut = cms.string("energy() >= 6.0")) #Added

# select HF+ towers above threshold
hfPosTowers = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("towersAboveThreshold"),
    cut = cms.string("eta() >= 3.0 && eta() <= 6.0")
)

# select HF- towers above threshold
hfNegTowers = cms.EDFilter("CandViewSelector",
    src = cms.InputTag("towersAboveThreshold"),
    cut = cms.string("eta() >= -6.0 && eta() <= -3.0")
)

# select HF+/HF- towers above threshold 4 and 5
hfPosTowersTh2 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh2"))
hfNegTowersTh2 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh2"))

hfPosTowersTh3 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh3"))
hfNegTowersTh3 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh3"))

hfPosTowersTh4 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh4"))
hfNegTowersTh4 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh4"))

hfPosTowersTh5 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh5"))
hfNegTowersTh5 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh5"))

hfPosTowersTh6 = hfPosTowers.clone(src=cms.InputTag("towersAboveThresholdTh6"))
hfNegTowersTh6 = hfNegTowers.clone(src=cms.InputTag("towersAboveThresholdTh6"))

# require at least one HF+ tower above threshold
hfPosFilter2 = cms.EDFilter("CandCountFilter",
    src = cms.InputTag("hfPosTowers"),
    minNumber = cms.uint32(2)
)

# require at least one HF- tower above threshold
hfNegFilter2 = cms.EDFilter("CandCountFilter",
    src = cms.InputTag("hfNegTowers"),
    minNumber = cms.uint32(2)
)

# two HF towers above threshold on each side
hfPosFilter1Th2 =hfPosFilter2.clone(src="hfPosTowersTh2", minNumber=1)
hfNegFilter1Th2 =hfNegFilter2.clone(src="hfNegTowersTh2", minNumber=1)
hfPosFilter2Th2 =hfPosFilter2.clone(src="hfPosTowersTh2", minNumber=2)
hfNegFilter2Th2 =hfNegFilter2.clone(src="hfNegTowersTh2", minNumber=2)
hfPosFilter3Th2 =hfPosFilter2.clone(src="hfPosTowersTh2", minNumber=3)
hfNegFilter3Th2 =hfNegFilter2.clone(src="hfNegTowersTh2", minNumber=3)
hfPosFilter4Th2 =hfPosFilter2.clone(src="hfPosTowersTh2", minNumber=4)
hfNegFilter4Th2 =hfNegFilter2.clone(src="hfNegTowersTh2", minNumber=4)

hfPosFilter1Th3 =hfPosFilter2.clone(src="hfPosTowersTh3", minNumber=1)
hfNegFilter1Th3 =hfNegFilter2.clone(src="hfNegTowersTh3", minNumber=1)
hfPosFilter2Th3 =hfPosFilter2.clone(src="hfPosTowersTh3", minNumber=2)
hfNegFilter2Th3 =hfNegFilter2.clone(src="hfNegTowersTh3", minNumber=2)
hfPosFilter3Th3 =hfPosFilter2.clone(src="hfPosTowersTh3", minNumber=3)
hfNegFilter3Th3 =hfNegFilter2.clone(src="hfNegTowersTh3", minNumber=3)
hfPosFilter4Th3 =hfPosFilter2.clone(src="hfPosTowersTh3", minNumber=4)
hfNegFilter4Th3 =hfNegFilter2.clone(src="hfNegTowersTh3", minNumber=4)

hfPosFilter1Th4 =hfPosFilter2.clone(src="hfPosTowersTh4", minNumber=1)
hfNegFilter1Th4 =hfNegFilter2.clone(src="hfNegTowersTh4", minNumber=1)
hfPosFilter2Th4 =hfPosFilter2.clone(src="hfPosTowersTh4", minNumber=2)
hfNegFilter2Th4 =hfNegFilter2.clone(src="hfNegTowersTh4", minNumber=2)
hfPosFilter3Th4 =hfPosFilter2.clone(src="hfPosTowersTh4", minNumber=3)
hfNegFilter3Th4 =hfNegFilter2.clone(src="hfNegTowersTh4", minNumber=3)
hfPosFilter4Th4 =hfPosFilter2.clone(src="hfPosTowersTh4", minNumber=4)
hfNegFilter4Th4 =hfNegFilter2.clone(src="hfNegTowersTh4", minNumber=4)

hfPosFilter1Th5 =hfPosFilter2.clone(src="hfPosTowersTh5", minNumber=1)
hfNegFilter1Th5 =hfNegFilter2.clone(src="hfNegTowersTh5", minNumber=1)
hfPosFilter2Th5 =hfPosFilter2.clone(src="hfPosTowersTh5", minNumber=2)
hfNegFilter2Th5 =hfNegFilter2.clone(src="hfNegTowersTh5", minNumber=2)
hfPosFilter3Th5 =hfPosFilter2.clone(src="hfPosTowersTh5", minNumber=3)
hfNegFilter3Th5 =hfNegFilter2.clone(src="hfNegTowersTh5", minNumber=3)
hfPosFilter4Th5 =hfPosFilter2.clone(src="hfPosTowersTh5", minNumber=4)
hfNegFilter4Th5 =hfNegFilter2.clone(src="hfNegTowersTh5", minNumber=4)

hfPosFilter1Th6 =hfPosFilter2.clone(src="hfPosTowersTh6", minNumber=1)
hfNegFilter1Th6 =hfNegFilter2.clone(src="hfNegTowersTh6", minNumber=1)
hfPosFilter2Th6 =hfPosFilter2.clone(src="hfPosTowersTh6", minNumber=2)
hfNegFilter2Th6 =hfNegFilter2.clone(src="hfNegTowersTh6", minNumber=2)
hfPosFilter3Th6 =hfPosFilter2.clone(src="hfPosTowersTh6", minNumber=3)
hfNegFilter3Th6 =hfNegFilter2.clone(src="hfNegTowersTh6", minNumber=3)
hfPosFilter4Th6 =hfPosFilter2.clone(src="hfPosTowersTh6", minNumber=4)
hfNegFilter4Th6 =hfNegFilter2.clone(src="hfNegTowersTh6", minNumber=4)



phfCoincFilter1Th2 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter1Th2 *
    hfNegFilter1Th2)
phfCoincFilter2Th2 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter2Th2 *
    hfNegFilter2Th2)
phfCoincFilter3Th2 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter3Th2 *
    hfNegFilter3Th2)
phfCoincFilter4Th2 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh2 *
    hfPosTowersTh2 *
    hfNegTowersTh2 *
    hfPosFilter4Th2 *
    hfNegFilter4Th2)


phfCoincFilter1Th3 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh3 *
    hfPosTowersTh3 *
    hfNegTowersTh3 *
    hfPosFilter1Th3 *
    hfNegFilter1Th3)
phfCoincFilter2Th3 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh3 *
    hfPosTowersTh3 *
    hfNegTowersTh3 *
    hfPosFilter2Th3 *
    hfNegFilter2Th3)
phfCoincFilter3Th3 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh3 *
    hfPosTowersTh3 *
    hfNegTowersTh3 *
    hfPosFilter3Th3 *
    hfNegFilter3Th3)
phfCoincFilter4Th3 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh3 *
    hfPosTowersTh3 *
    hfNegTowersTh3 *
    hfPosFilter4Th3 *
    hfNegFilter4Th3)

phfCoincFilter1Th4 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter1Th4 *
    hfNegFilter1Th4)
phfCoincFilter2Th4 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter2Th4 *
    hfNegFilter2Th4)
phfCoincFilter3Th4 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter3Th4 *
    hfNegFilter3Th4)
phfCoincFilter4Th4 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh4 *
    hfPosTowersTh4 *
    hfNegTowersTh4 *
    hfPosFilter4Th4 *
    hfNegFilter4Th4)

phfCoincFilter1Th5 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter1Th5 *
    hfNegFilter1Th5)
phfCoincFilter2Th5 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter2Th5 *
    hfNegFilter2Th5)
phfCoincFilter3Th5 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter3Th5 *
    hfNegFilter3Th5)
phfCoincFilter4Th5 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh5 *
    hfPosTowersTh5 *
    hfNegTowersTh5 *
    hfPosFilter4Th5 *
    hfNegFilter4Th5)

phfCoincFilter1Th6 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh6 *
    hfPosTowersTh6 *
    hfNegTowersTh6 *
    hfPosFilter1Th6 *
    hfNegFilter1Th6)
phfCoincFilter2Th6 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh6 *
    hfPosTowersTh6 *
    hfNegTowersTh6 *
    hfPosFilter2Th6 *
    hfNegFilter2Th6)
phfCoincFilter3Th6 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh6 *
    hfPosTowersTh6 *
    hfNegTowersTh6 *
    hfPosFilter3Th6 *
    hfNegFilter3Th6)
phfCoincFilter4Th6 = cms.Sequence(
    hfPFCandidates *
    towersAboveThresholdTh6 *
    hfPosTowersTh6 *
    hfNegTowersTh6 *
    hfPosFilter4Th6 *
    hfNegFilter4Th6)


