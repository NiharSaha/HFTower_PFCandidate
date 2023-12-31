#ifndef PFCand_AssoMapAlgos_h
#define PFCand_AssoMapAlgos_h

/**\class PF_PU_AssoMap PF_PU_AssoMap.cc CommonTools/RecoUtils/plugins/PF_PU_AssoMap.cc

 Description: Produces a map with association between tracks and their particular most probable vertex with a quality of this association
*/
//
// Original Author:  Matthias Geisler,32 4-B20,+41227676487,
// $Id$
//
//
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "CommonTools/RecoUtils/interface/PF_PU_AssoMapAlgos.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

namespace edm {
  class EDProductGetter;
}

//
// constants, enums and typedefs
//

typedef edm::AssociationMap<edm::OneToManyWithQuality<reco::VertexCollection, reco::PFCandidateCollection, int>>
    PFCandToVertexAssMap;
typedef edm::AssociationMap<edm::OneToManyWithQuality<reco::PFCandidateCollection, reco::VertexCollection, int>>
    VertexToPFCandAssMap;

typedef std::pair<reco::PFCandidateRef, int> PFCandQualityPair;
typedef std::vector<PFCandQualityPair> PFCandQualityPairVector;

typedef std::pair<reco::VertexRef, PFCandQualityPair> VertexPfcQuality;

typedef std::pair<reco::VertexRef, float> VertexPtsumPair;
typedef std::vector<VertexPtsumPair> VertexPtsumVector;

class PFCand_AssoMapAlgos : public PF_PU_AssoMapAlgos {
public:
  //dedicated constructor for the algorithms
  PFCand_AssoMapAlgos(const edm::ParameterSet&, edm::ConsumesCollector&&);

  //get all needed collections at the beginning
  void GetInputCollections(edm::Event&, const edm::EventSetup&) override;

  //create the pf candidate to vertex association and the inverse map
  std::pair<std::unique_ptr<PFCandToVertexAssMap>, std::unique_ptr<VertexToPFCandAssMap>> createMappings(
      edm::Handle<reco::PFCandidateCollection> pfCandH);

  //create the pf candidate to vertex association map
  std::unique_ptr<PFCandToVertexAssMap> CreatePFCandToVertexMap(edm::Handle<reco::PFCandidateCollection>);

  //create the vertex to pf candidate association map
  std::unique_ptr<VertexToPFCandAssMap> CreateVertexToPFCandMap(edm::Handle<reco::PFCandidateCollection>);

  //function to sort the vertices in the AssociationMap by the sum of (pT - pT_Error)**2
  std::unique_ptr<PFCandToVertexAssMap> SortPFCandAssociationMap(PFCandToVertexAssMap*,
                                                                 edm::EDProductGetter const* getter);

protected:
  //protected functions

private:
  //private functions

  int input_MaxNumAssociations_;

  edm::EDGetTokenT<reco::VertexCollection> token_VertexCollection_;
  edm::Handle<reco::VertexCollection> vtxcollH;

  edm::EDGetTokenT<reco::BeamSpot> token_BeamSpot_;
  edm::Handle<reco::BeamSpot> beamspotH;

  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> token_bField_;
  edm::ESHandle<MagneticField> bFieldH;
  const edm::ESGetToken<GlobalTrackingGeometry, GlobalTrackingGeometryRecord> token_TrackingGeometry_;
  edm::ESHandle<GlobalTrackingGeometry> trackingGeometryH;
};

#endif
