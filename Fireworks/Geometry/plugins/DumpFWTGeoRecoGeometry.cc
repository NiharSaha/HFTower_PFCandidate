#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "Fireworks/Geometry/interface/FWTGeoRecoGeometry.h"
#include "Fireworks/Geometry/interface/FWTGeoRecoGeometryRecord.h"

#include "TGeoManager.h"
#include "TFile.h"
#include "TTree.h"
#include "TError.h"
#include "TSystem.h"

class DumpFWTGeoRecoGeometry : public edm::one::EDAnalyzer<> {
public:
  explicit DumpFWTGeoRecoGeometry(const edm::ParameterSet& config);
  ~DumpFWTGeoRecoGeometry(void) override {}

private:
  void analyze(const edm::Event& event, const edm::EventSetup& eventSetup) override;
  void beginJob(void) override;
  void endJob(void) override;

  std::string m_tag;
  std::string m_outputFileName;
  const edm::ESGetToken<FWTGeoRecoGeometry, FWTGeoRecoGeometryRecord> m_geomToken;
};

DumpFWTGeoRecoGeometry::DumpFWTGeoRecoGeometry(const edm::ParameterSet& config)
    : m_tag(config.getUntrackedParameter<std::string>("tagInfo", "unknown")),
      m_outputFileName(config.getUntrackedParameter<std::string>("outputFileName", "cmsTGeoReco.root")),
      m_geomToken(esConsumes()) {}

void DumpFWTGeoRecoGeometry::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) {
  using namespace edm;

  ESTransientHandle<FWTGeoRecoGeometry> geoh = eventSetup.getTransientHandle(m_geomToken);
  TGeoManager* geom = geoh.product()->manager();  //const_cast<TGeoManager*>( geoh.product()->manager());

  TFile file(m_outputFileName.c_str(), "RECREATE");
  file.WriteTObject(&*geom);
  file.WriteTObject(new TNamed("CMSSW_VERSION", gSystem->Getenv("CMSSW_VERSION")));
  file.WriteTObject(new TNamed("tag", m_tag.c_str()));
  file.Close();
}

void DumpFWTGeoRecoGeometry::beginJob(void) {}

void DumpFWTGeoRecoGeometry::endJob(void) {}

DEFINE_FWK_MODULE(DumpFWTGeoRecoGeometry);
