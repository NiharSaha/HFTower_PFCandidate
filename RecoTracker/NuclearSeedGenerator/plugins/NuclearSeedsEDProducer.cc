#include "RecoTracker/NuclearSeedGenerator/interface/NuclearSeedsEDProducer.h"
#include "RecoTracker/NuclearSeedGenerator/interface/NuclearInteractionFinder.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TrackingTools/KalmanUpdators/interface/Chi2MeasurementEstimatorBase.h"
#include "RecoTracker/Record/interface/NavigationSchoolRecord.h"
#include "RecoTracker/Record/interface/CkfComponentsRecord.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"

using namespace edm;
using namespace std;
using namespace reco;

//
// constructors and destructor
//
NuclearSeedsEDProducer::NuclearSeedsEDProducer(const edm::ParameterSet& iConfig)
    : config_(),
      improveSeeds(iConfig.getParameter<bool>("improveSeeds")),
      producer_(consumes<TrajectoryCollection>(iConfig.getParameter<std::string>("producer"))),
      mteToken_(consumes<MeasurementTrackerEvent>(edm::InputTag("MeasurementTrackerEvents"))) {
  produces<TrajectorySeedCollection>();
  produces<TrajectoryToSeedsMap>();

  trackerGeomToken_ = esConsumes<edm::Transition::BeginRun>();
  propagatorToken_ = esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", "PropagatorWithMaterial"));
  estimatorToken_ = esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", "Chi2"));

  std::string measurementTrackerName = iConfig.getParameter<std::string>("MeasurementTrackerName");
  measurementTrackerToken_ = esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", measurementTrackerName));
  geomSearchTrackerToken_ = esConsumes<edm::Transition::BeginRun>();

  auto navigationSchoolName = iConfig.getParameter<std::string>("NavigationSchool");
  navigationToken_ = esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", navigationSchoolName));

  config_.maxHits = iConfig.getParameter<int>("maxHits");
  config_.rescaleErrorFactor = iConfig.getParameter<double>("rescaleErrorFactor");
  config_.checkCompletedTrack = iConfig.getParameter<bool>("checkCompletedTrack");
  config_.ptMin = iConfig.getParameter<double>("ptMin");
}

NuclearSeedsEDProducer::~NuclearSeedsEDProducer() {}

//
// member functions
//

// ------------ method called to produce the data  ------------
void NuclearSeedsEDProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  typedef TrajectoryMeasurement TM;

  edm::Handle<TrajectoryCollection> m_TrajectoryCollection;
  iEvent.getByToken(producer_, m_TrajectoryCollection);

  LogDebug("NuclearSeedGenerator") << "Number of trajectory in event :" << m_TrajectoryCollection->size() << "\n";

  auto output = std::make_unique<TrajectorySeedCollection>();
  auto outAssoc = std::make_unique<TrajectoryToSeedsMap>();

  edm::Handle<MeasurementTrackerEvent> data;
  iEvent.getByToken(mteToken_, data);

  //   NavigationSetter setter( *(theNuclearInteractionFinder->nav()) );   why???

  std::vector<std::pair<int, int> > assocPair;
  int i = 0;

  for (std::vector<Trajectory>::const_iterator iTraj = m_TrajectoryCollection->begin();
       iTraj != m_TrajectoryCollection->end();
       iTraj++, i++) {
    // run the finder
    theNuclearInteractionFinder->run(*iTraj, *data);

    // improve seeds
    if (improveSeeds == true)
      theNuclearInteractionFinder->improveSeeds(*data);

    // push back the new persistent seeds in output
    std::unique_ptr<TrajectorySeedCollection> newSeeds(theNuclearInteractionFinder->getPersistentSeeds());
    output->insert(output->end(), newSeeds->begin(), newSeeds->end());

    // fill the id of the Trajectory and the if of the seed in assocPair
    for (unsigned int j = 0; j < newSeeds->size(); j++) {
      assocPair.push_back(std::make_pair(i, output->size() - newSeeds->size() + j));
    }
  }

  const edm::OrphanHandle<TrajectorySeedCollection> refprodTrajSeedColl = iEvent.put(std::move(output));

  for (std::vector<std::pair<int, int> >::const_iterator iVecP = assocPair.begin(); iVecP != assocPair.end(); iVecP++) {
    outAssoc->insert(edm::Ref<TrajectoryCollection>(m_TrajectoryCollection, iVecP->first),
                     edm::Ref<TrajectorySeedCollection>(refprodTrajSeedColl, iVecP->second));
  }
  iEvent.put(std::move(outAssoc));
}

// ------------ method called once each job just before starting event loop  ------------
void NuclearSeedsEDProducer::beginRun(edm::Run const& run, const edm::EventSetup& es) {
  theNuclearInteractionFinder = std::make_unique<NuclearInteractionFinder>(config_,
                                                                           &es.getData(trackerGeomToken_),
                                                                           &es.getData(propagatorToken_),
                                                                           &es.getData(estimatorToken_),
                                                                           &es.getData(measurementTrackerToken_),
                                                                           &es.getData(geomSearchTrackerToken_),
                                                                           &es.getData(navigationToken_));
}

DEFINE_FWK_MODULE(NuclearSeedsEDProducer);
