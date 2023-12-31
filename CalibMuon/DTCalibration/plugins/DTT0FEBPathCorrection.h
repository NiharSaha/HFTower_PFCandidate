#ifndef CalibMuon_DTT0FEBPathCorrection_H
#define CalibMuon_DTT0FEBPathCorrection_H

/** \class DTT0FEBPathCorrection
 *  Concrete implementation of a DTT0BaseCorrection.
 *  Computes correction for t0 for FEB path differences
 *
 *  $Revision: 1.1 $
 *  \author Mark Olschewski
 */

#include "CalibMuon/DTCalibration/interface/DTT0BaseCorrection.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "CondFormats/DataRecord/interface/DTT0Rcd.h"
#include "CondFormats/DTObjects/interface/DTT0.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include <string>

namespace edm {
  class ParameterSet;
}

class DTT0;

namespace dtCalibration {

  class DTT0FEBPathCorrection : public DTT0BaseCorrection {
  public:
    // Constructor
    DTT0FEBPathCorrection(const edm::ParameterSet&, edm::ConsumesCollector);

    // Destructor
    ~DTT0FEBPathCorrection() override;

    void setES(const edm::EventSetup& setup) override;
    DTT0Data correction(const DTWireId&) override;

    float t0FEBPathCorrection(int wheel, int st, int sec, int sl, int l, int w);

  private:
    DTT0Data defaultT0(const DTWireId&);

    std::string calibChamber_;

    DTChamberId chosenChamberId_;
    const DTT0* t0Map_;
    edm::ESGetToken<DTT0, DTT0Rcd> t0Token_;
  };

}  // namespace dtCalibration
#endif
