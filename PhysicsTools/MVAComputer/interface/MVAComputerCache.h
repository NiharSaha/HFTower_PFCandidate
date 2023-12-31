#ifndef PhysicsTools_MVAComputer_MVAComputerCache_h
#define PhysicsTools_MVAComputer_MVAComputerCache_h
// -*- C++ -*-
//
// Package:     MVAComputerCache
// Class  :     MVAComputerCache
//

//
// Author:	Christophe Saout <christophe.saout@cern.ch>
// Created:     Sat Feb 23 15:38 CEST 2007
//

#include <memory>

#include "PhysicsTools/MVAComputer/interface/Calibration.h"
#include "PhysicsTools/MVAComputer/interface/MVAComputer.h"

namespace PhysicsTools {

  /** \class MVAComputerCache
 *
 * \short Creates and and MVAComputer from calibrations via EventSetup
 *
 ************************************************************/
  class MVAComputerCache {
  public:
    MVAComputerCache();
    ~MVAComputerCache();

    bool update(const Calibration::MVAComputer *computer);
    bool update(const Calibration::MVAComputerContainer *container, const char *calib);

    operator bool() const { return computer.get(); }

    MVAComputer &operator*() { return *computer; }
    const MVAComputer &operator*() const { return *computer; }

    MVAComputer *operator->() { return computer.get(); }
    const MVAComputer *operator->() const { return computer.get(); }

    MVAComputer *get() { return computer.get(); }
    const MVAComputer *get() const { return computer.get(); }

    std::unique_ptr<MVAComputer> release();

    void reset() { computer.reset(); }

  private:
    Calibration::MVAComputerContainer::CacheId containerCacheId;
    Calibration::MVAComputer::CacheId computerCacheId;
    std::unique_ptr<MVAComputer> computer;
  };

}  // namespace PhysicsTools

#endif  // PhysicsTools_MVAComputer_MVAComputerCache_h
