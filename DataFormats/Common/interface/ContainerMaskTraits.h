#ifndef DataFormats_Common_ContainerMaskTraits_h
#define DataFormats_Common_ContainerMaskTraits_h
// -*- C++ -*-
//
// Package:     Common
// Class  :     ContainerMaskTraits
//
/**\class ContainerMaskTraits ContainerMaskTraits.h DataFormats/Common/interface/ContainerMaskTraits.h

 Description: Helper class for ContainerMask which allows for adapting for different container types

 Usage:
    <usage>

*/
//
// Original Author:
//         Created:  Fri Sep 23 17:05:48 CDT 2011
//

// system include files
#include <cstddef>

// user include files

// forward declarations
namespace edm {
  template <typename T>
  class ContainerMaskTraits {
  public:
    typedef typename T::value_type value_type;

    static size_t size(const T* iContainer) { return iContainer->size(); }
    static unsigned int indexFor(const value_type* iElement, const T* iContainer) {
      return iElement - &(iContainer->front());
    }

    ContainerMaskTraits() = delete;
    ContainerMaskTraits(const ContainerMaskTraits&) = delete;                   // stop default
    const ContainerMaskTraits& operator=(const ContainerMaskTraits&) = delete;  // stop default

  private:
    //virtual ~ContainerMaskTraits();

    // ---------- member data --------------------------------
  };
}  // namespace edm

#endif
