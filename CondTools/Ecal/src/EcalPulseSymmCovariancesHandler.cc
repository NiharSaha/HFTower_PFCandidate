#include "CondTools/Ecal/interface/EcalPulseSymmCovariancesHandler.h"

#include <iostream>
#include <fstream>

popcon::EcalPulseSymmCovariancesHandler::EcalPulseSymmCovariancesHandler(const edm::ParameterSet& ps)
    : m_name(ps.getUntrackedParameter<std::string>("name", "EcalPulseSymmCovariancesHandler")) {
  std::cout << "EcalPulseSymmCovariance Source handler constructor\n" << std::endl;
  m_firstRun = static_cast<unsigned int>(atoi(ps.getParameter<std::string>("firstRun").c_str()));
  m_filename = ps.getParameter<std::string>("inputFileName");
  m_EBPulseShapeCovariance = ps.getParameter<std::vector<double> >("EBPulseShapeCovariance");
  m_EEPulseShapeCovariance = ps.getParameter<std::vector<double> >("EEPulseShapeCovariance");
}

popcon::EcalPulseSymmCovariancesHandler::~EcalPulseSymmCovariancesHandler() {}

bool popcon::EcalPulseSymmCovariancesHandler::checkPulseSymmCovariance(EcalPulseSymmCovariances::Item* item) {
  // true means all is standard and OK
  bool result = true;
  int N = EcalPulseShape::TEMPLATESAMPLES * (EcalPulseShape::TEMPLATESAMPLES + 1) / 2;
  for (int k = 0; k < N; ++k) {
    if (fabs(item->covval[k]) > 1)
      result = false;
  }
  return result;
}

void popcon::EcalPulseSymmCovariancesHandler::fillSimPulseSymmCovariance(EcalPulseSymmCovariances::Item* item,
                                                                         bool isbarrel) {
  for (int k = 0; k < std::pow(EcalPulseShape::TEMPLATESAMPLES, 2); ++k) {
    int i = k / EcalPulseShape::TEMPLATESAMPLES;
    int j = k % EcalPulseShape::TEMPLATESAMPLES;
    if (j >= i) {
      int linK = j + (EcalPulseShape::TEMPLATESAMPLES - 1) * i;
      item->covval[linK] = isbarrel ? m_EBPulseShapeCovariance[k] : m_EEPulseShapeCovariance[k];
    }
  }
}

void popcon::EcalPulseSymmCovariancesHandler::getNewObjects() {
  std::cout << "------- Ecal - > getNewObjects\n";

  // create the object pukse shapes
  EcalPulseSymmCovariances* pulsecovs = new EcalPulseSymmCovariances();

  // read the templates from a text file
  std::ifstream inputfile;
  inputfile.open(m_filename.c_str());
  float covvals[EcalPulseShape::TEMPLATESAMPLES][EcalPulseShape::TEMPLATESAMPLES];
  unsigned int rawId;
  int isbarrel;
  std::string line;

  // keep track of bad crystals
  int nEBbad(0), nEEbad(0);
  std::vector<EBDetId> ebgood;
  std::vector<EEDetId> eegood;

  // fill with the measured shapes only for data
  if (m_firstRun > 1) {
    while (std::getline(inputfile, line)) {
      std::istringstream linereader(line);
      linereader >> isbarrel >> rawId;
      // std::cout << "Inserting template for crystal with rawId = " << rawId << " (isbarrel = " << isbarrel << ") " << std::endl;
      for (int k = 0; k < std::pow(EcalPulseShape::TEMPLATESAMPLES, 2); ++k) {
        int i = k / EcalPulseShape::TEMPLATESAMPLES;
        int j = k % EcalPulseShape::TEMPLATESAMPLES;
        linereader >> covvals[i][j];
      }

      if (!linereader) {
        std::cout << "Wrong format of the text file. Exit." << std::endl;
        return;
      }
      EcalPulseSymmCovariances::Item item;
      for (int i = 0; i < EcalPulseShape::TEMPLATESAMPLES; ++i)
        for (int j = 0; j < EcalPulseShape::TEMPLATESAMPLES; ++j) {
          item.val(i, j) = covvals[i][j];
        }

      if (isbarrel) {
        EBDetId ebdetid(rawId);
        if (!checkPulseSymmCovariance(&item))
          nEBbad++;
        else {
          ebgood.push_back(ebdetid);
          pulsecovs->insert(std::make_pair(ebdetid.rawId(), item));
        }
      } else {
        EEDetId eedetid(rawId);
        if (!checkPulseSymmCovariance(&item))
          nEEbad++;
        else {
          eegood.push_back(eedetid);
          pulsecovs->insert(std::make_pair(eedetid.rawId(), item));
        }
      }
    }
  }

  // now fill the bad crystals and simulation with the simulation values (from TB)
  std::cout << "Filled the DB with the good measured ECAL templates. Now filling the others with the TB values"
            << std::endl;
  for (int iEta = -EBDetId::MAX_IETA; iEta <= EBDetId::MAX_IETA; ++iEta) {
    if (iEta == 0)
      continue;
    for (int iPhi = EBDetId::MIN_IPHI; iPhi <= EBDetId::MAX_IPHI; ++iPhi) {
      if (EBDetId::validDetId(iEta, iPhi)) {
        EBDetId ebdetid(iEta, iPhi, EBDetId::ETAPHIMODE);

        std::vector<EBDetId>::iterator it = find(ebgood.begin(), ebgood.end(), ebdetid);
        if (it == ebgood.end()) {
          EcalPulseSymmCovariances::Item item;
          fillSimPulseSymmCovariance(&item, true);
          pulsecovs->insert(std::make_pair(ebdetid.rawId(), item));
        }
      }
    }
  }

  for (int iZ = -1; iZ < 2; iZ += 2) {
    for (int iX = EEDetId::IX_MIN; iX <= EEDetId::IX_MAX; ++iX) {
      for (int iY = EEDetId::IY_MIN; iY <= EEDetId::IY_MAX; ++iY) {
        if (EEDetId::validDetId(iX, iY, iZ)) {
          EEDetId eedetid(iX, iY, iZ);

          std::vector<EEDetId>::iterator it = find(eegood.begin(), eegood.end(), eedetid);
          if (it == eegood.end()) {
            EcalPulseSymmCovariances::Item item;
            fillSimPulseSymmCovariance(&item, false);
            pulsecovs->insert(std::make_pair(eedetid.rawId(), item));
          }
        }
      }
    }
  }

  std::cout << "Inserted the pulse covariances into the new item object" << std::endl;

  unsigned int irun = m_firstRun;
  Time_t snc = (Time_t)irun;

  m_to_transfer.push_back(std::make_pair((EcalPulseSymmCovariances*)pulsecovs, snc));

  std::cout << "Ecal - > end of getNewObjects -----------" << std::endl;
  std::cout << "N. bad shapes for EB = " << nEBbad << std::endl;
  std::cout << "N. bad shapes for EE = " << nEEbad << std::endl;
  std::cout << "Written the object" << std::endl;
}
