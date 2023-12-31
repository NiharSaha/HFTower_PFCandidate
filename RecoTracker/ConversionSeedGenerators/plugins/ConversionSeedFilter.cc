// -*- C++ -*-
//
// Package:    ConversionSeedFilter
// Class:      ConversionSeedFilter
//
/**\class ConversionSeedFilter ConversionSeedFilter.cc RecoTracker/TkSeedGenerator/src/ConversionSeedFilter.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Giuseppe Cerati & Domenico Giordano
//         Created:  Thu Mar 11 10:48:48 CET 2010
//
//

#include <memory>
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

class ConversionSeedFilter : public edm::stream::EDProducer<> {
public:
  explicit ConversionSeedFilter(const edm::ParameterSet&);
  ~ConversionSeedFilter() override = default;

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
  bool isCompatible(double* vars1, double* vars2);
  void getKine(const TrajectoryStateOnSurface& tsos, double* vars);
  void SearchAmongSeeds(const TrajectorySeedCollection* pInPos,
                        const TrajectorySeedCollection* pInNeg,
                        TrajectorySeedCollection& selectedColl,
                        std::vector<bool>& idxPosColl1,
                        std::vector<bool>& idxPosColl2);
  void SearchAmongTracks(const TrajectorySeedCollection* pInSeed,
                         const reco::TrackCollection* pInTk,
                         TrajectorySeedCollection& selectedColl,
                         std::vector<bool>& idxPosColl);
  void SearchAmongTrajectories(const TrajectorySeedCollection* pInSeed,
                               const Trajectory* InTj,
                               TrajectorySeedCollection& selectedColl,
                               std::vector<bool>& idxPosColl);

  TrajectoryStateOnSurface getTSOS(const TrajectorySeed& ts);
  TrajectoryStateOnSurface getTSOS(const reco::Track& tk);
  TrajectoryStateOnSurface getTSOS(const Trajectory& tj, const TrajectorySeed& ts);

  const edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> geomToken;
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> mfToken;
  const edm::EDGetTokenT<TrajectorySeedCollection> inputCollSeedPos;
  const edm::EDGetTokenT<TrajectorySeedCollection> inputCollSeedNeg;
  const edm::EDGetTokenT<TrajTrackAssociationCollection> inputTrajectory;
  const double deltaPhiCut, deltaCotThetaCut, deltaRCut, deltaZCut;

  const TrackerGeometry* theG;
  const MagneticField* theMF;
  uint32_t maxInputSeeds;
  bool takeAll;
};

ConversionSeedFilter::ConversionSeedFilter(const edm::ParameterSet& cfg)
    : geomToken(esConsumes()),
      mfToken(esConsumes()),
      //inputCollTkPos(cfg.getParameter<edm::InputTag>("tkCollectionPos")),
      inputCollSeedPos(consumes<TrajectorySeedCollection>(cfg.getParameter<edm::InputTag>("seedCollectionPos"))),
      //inputCollTkNeg(cfg.getParameter<edm::InputTag>("tkCollectionNeg")),
      inputCollSeedNeg(consumes<TrajectorySeedCollection>(cfg.getParameter<edm::InputTag>("seedCollectionNeg"))),
      inputTrajectory(consumes<TrajTrackAssociationCollection>(cfg.getParameter<edm::InputTag>("inputTrajectory"))),
      deltaPhiCut(cfg.getParameter<double>("deltaPhiCut")),
      deltaCotThetaCut(cfg.getParameter<double>("deltaCotThetaCut")),
      deltaRCut(cfg.getParameter<double>("deltaRCut")),
      deltaZCut(cfg.getParameter<double>("deltaZCut")),
      maxInputSeeds(cfg.getParameter<uint32_t>("maxInputSeeds")),
      takeAll(cfg.getParameter<bool>("takeAll")) {
  produces<TrajectorySeedCollection>();
}

void ConversionSeedFilter::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;
  Handle<TrajectorySeedCollection> pInPos;
  iEvent.getByToken(inputCollSeedPos, pInPos);
  Handle<TrajectorySeedCollection> pInNeg;
  iEvent.getByToken(inputCollSeedNeg, pInNeg);

  // Handle<reco::TrackCollection> pInTkPos;  iEvent.getByLabel(inputCollTkPos,pInTkPos);
  // Handle<reco::TrackCollection> pInTkNeg;  iEvent.getByLabel(inputCollTkNeg,pInTkNeg);

  edm::Handle<TrajTrackAssociationCollection> trajTrackAssociations;
  iEvent.getByToken(inputTrajectory, trajTrackAssociations);

  theG = &iSetup.getData(geomToken);
  theMF = &iSetup.getData(mfToken);

  auto result = std::make_unique<TrajectorySeedCollection>();

  TrajectorySeedCollection selectedColl;

  if (takeAll) {
    result->insert(result->end(), pInPos->begin(), pInPos->end());
    result->insert(result->end(), pInNeg->begin(), pInNeg->end());
  } else {
    edm::LogInfo("ConversionSeedFilter") << "takeAll " << takeAll;
    if (pInPos->size() < maxInputSeeds && pInNeg->size() < maxInputSeeds) {
      std::vector<bool> idxPosColl1(pInPos->size(), false);
      std::vector<bool> idxPosColl2(pInNeg->size(), false);
      selectedColl.reserve(pInPos->size());

      edm::LogInfo("ConversionSeedFilter")
          << "New Event \t Pos " << pInPos->size() << " \t Neg " << pInNeg->size() << std::endl;

      SearchAmongSeeds(pInPos.product(), pInNeg.product(), selectedColl, idxPosColl1, idxPosColl2);
      //SearchAmongTracks(pInPos.product(),pInTkNeg.product(),selectedColl,idxPosColl1);
      //SearchAmongTracks(pInNeg.product(),pInTkPos.product(),selectedColl,idxPosColl2);

      if (trajTrackAssociations.isValid()) {
        edm::LogInfo("ConversionSeedFilter") << "Reconstructed tracks " << trajTrackAssociations->size() << std::endl;
        for (TrajTrackAssociationCollection::const_iterator association = trajTrackAssociations->begin();
             association != trajTrackAssociations->end();
             association++) {
          const Trajectory* traj = association->key.get();
          const reco::Track* track = association->val.get();

          //edm::LogInfo("ConversionSeedFilter") << "Traj charge " << track->charge() << std::endl;

          if (track->charge() < 0) {
            SearchAmongTrajectories(pInPos.product(), traj, selectedColl, idxPosColl1);
          } else {
            SearchAmongTrajectories(pInNeg.product(), traj, selectedColl, idxPosColl2);
          }
        }
      }
    }
    result->insert(result->end(), selectedColl.begin(), selectedColl.end());
  }

  edm::LogInfo("ConversionSeedFilter") << "\nNew Event : result size " << result->size() << std::endl;

  iEvent.put(std::move(result));
}

void ConversionSeedFilter::SearchAmongSeeds(const TrajectorySeedCollection* pInPos,
                                            const TrajectorySeedCollection* pInNeg,
                                            TrajectorySeedCollection& selectedColl,
                                            std::vector<bool>& idxPosColl1,
                                            std::vector<bool>& idxPosColl2) {
  for (TrajectorySeedCollection::const_iterator iS1 = pInPos->begin(); iS1 != pInPos->end(); ++iS1) {
    bool pushed1 = false;

    double vars1[4];
    getKine(getTSOS(*iS1), vars1);

    for (TrajectorySeedCollection::const_iterator iS2 = pInNeg->begin(); iS2 != pInNeg->end(); ++iS2) {
      double vars2[4];
      getKine(getTSOS(*iS2), vars2);

      if (isCompatible(vars1, vars2)) {
        edm::LogInfo("ConversionSeedFilter")
            << "[SearchAmongSeeds] match in pos " << iS1 - pInPos->begin() << " " << iS2 - pInNeg->begin() << std::endl;
        if (!pushed1) {
          idxPosColl1[iS1 - pInPos->begin()] = true;
          selectedColl.push_back(*iS1);
          pushed1 = true;
        }
        if (!idxPosColl2[iS2 - pInNeg->begin()]) {
          selectedColl.push_back(*iS2);
          idxPosColl2[iS2 - pInNeg->begin()] = true;
        }
      }
    }
  }
}

void ConversionSeedFilter::SearchAmongTracks(const TrajectorySeedCollection* pInSeed,
                                             const reco::TrackCollection* pInTk,
                                             TrajectorySeedCollection& selectedColl,
                                             std::vector<bool>& idxPosColl) {
  for (TrajectorySeedCollection::const_iterator iS1 = pInSeed->begin(); iS1 != pInSeed->end(); ++iS1) {
    if (idxPosColl[iS1 - pInSeed->begin()])
      continue;

    double vars1[4];
    getKine(getTSOS(*iS1), vars1);

    for (reco::TrackCollection::const_iterator iS2 = pInTk->begin(); iS2 != pInTk->end(); ++iS2) {
      double vars2[4];
      getKine(getTSOS(*iS2), vars2);

      if (isCompatible(vars1, vars2)) {
        edm::LogInfo("ConversionSeedFilter")
            << "[SearchAmongTracks] match in pos " << iS1 - pInSeed->begin() << std::endl;
        idxPosColl[iS1 - pInSeed->begin()] = true;
        selectedColl.push_back(*iS1);
        continue;
      }
    }
  }
}

void ConversionSeedFilter::SearchAmongTrajectories(const TrajectorySeedCollection* pInSeed,
                                                   const Trajectory* InTj,
                                                   TrajectorySeedCollection& selectedColl,
                                                   std::vector<bool>& idxPosColl) {
  for (TrajectorySeedCollection::const_iterator iS1 = pInSeed->begin(); iS1 != pInSeed->end(); ++iS1) {
    if (idxPosColl[iS1 - pInSeed->begin()])
      continue;

    double vars1[4];
    getKine(getTSOS(*iS1), vars1);

    double vars2[4];
    getKine(getTSOS(*InTj, *iS1), vars2);

    if (isCompatible(vars1, vars2)) {
      edm::LogInfo("ConversionSeedFilter")
          << "[SearchAmongTrajectories] match seed in pos " << iS1 - pInSeed->begin() << " of " << pInSeed->size()
          << " seed charge " << iS1->startingState().parameters().charge() << std::endl;
      idxPosColl[iS1 - pInSeed->begin()] = true;
      selectedColl.push_back(*iS1);
    }
  }
}

TrajectoryStateOnSurface ConversionSeedFilter::getTSOS(const TrajectorySeed& ts) {
  PTrajectoryStateOnDet state = ts.startingState();
  DetId detId(state.detId());
  return trajectoryStateTransform::transientState(state, &(theG->idToDet(detId)->surface()), theMF);
}

TrajectoryStateOnSurface ConversionSeedFilter::getTSOS(const reco::Track& tk) {
  return trajectoryStateTransform::innerStateOnSurface(tk, *theG, theMF);
}

TrajectoryStateOnSurface ConversionSeedFilter::getTSOS(const Trajectory& tj, const TrajectorySeed& ts) {
  PTrajectoryStateOnDet state = ts.startingState();
  DetId detId(state.detId());
  GlobalPoint p = theG->idToDet(detId)->surface().toGlobal(state.parameters().position());
  return tj.closestMeasurement(p).updatedState();
}

bool ConversionSeedFilter::isCompatible(double* vars1, double* vars2) {
  double deltaPhi = fabs(reco::deltaPhi(vars1[0], vars2[0]));
  double deltaCotTheta = fabs(vars1[1] - vars2[1]);
  double deltaR = fabs(vars1[2] - vars2[2]);
  double deltaZ = fabs(vars1[3] - vars2[3]);
  /*
  std::cout << "\nnewCheck" << std::endl;
  if(deltaPhi>deltaPhiCut)             std::cout << "\nko deltaphi" << deltaPhi; 
  if(deltaCotTheta>deltaCotThetaCut )  std::cout << "\nko deltaCt " << deltaCotTheta; 
  if(deltaR>deltaRCut 		    )  std::cout << "\nko deltaR  " << deltaR; 
  if(deltaZ>deltaZCut               )  std::cout << "\nko deltaZ  " << deltaZ; 
  if(deltaPhi<deltaPhiCut && deltaCotTheta<deltaCotThetaCut && deltaR<deltaRCut && deltaZ<deltaZCut) std::cout << "\nok :)\n";
  */
  return deltaPhi < deltaPhiCut && deltaCotTheta < deltaCotThetaCut && deltaR < deltaRCut && deltaZ < deltaZCut;
}

void ConversionSeedFilter::getKine(const TrajectoryStateOnSurface& tsos, double* vars) {
  vars[0] = tsos.globalMomentum().phi();             //phi
  vars[1] = 2 / tan(tsos.globalMomentum().theta());  //cotTheta
  vars[2] = tsos.globalPosition().perp();            //R
  vars[3] = tsos.globalPosition().z();               //Z
}

DEFINE_FWK_MODULE(ConversionSeedFilter);
