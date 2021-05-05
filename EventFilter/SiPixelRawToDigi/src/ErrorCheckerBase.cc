#include "EventFilter/SiPixelRawToDigi/interface/ErrorCheckerBase.h"

#include "CondFormats/SiPixelObjects/interface/SiPixelFrameConverter.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <bitset>
#include <sstream>
#include <iostream>

using namespace std;
using namespace edm;
using namespace sipixelobjects;
using namespace sipixelconstants;

ErrorCheckerBase::ErrorCheckerBase() : includeErrors(false) {}

void ErrorCheckerBase::setErrorStatus(bool ErrorStatus) { includeErrors = ErrorStatus; }

bool ErrorCheckerBase::checkCRC(bool& errorsInEvent, int fedId, const Word64* trailer, Errors& errors) {
  int CRC_BIT = (*trailer >> CRC_shift) & CRC_mask;
  if (CRC_BIT == 0)
    return true;
  errorsInEvent = true;
  if (includeErrors) {
    int errorType = 39;
    SiPixelRawDataError error(*trailer, errorType, fedId);
    errors[dummyDetId].push_back(error);
  }
  return false;
}

bool ErrorCheckerBase::checkHeader(bool& errorsInEvent, int fedId, const Word64* header, Errors& errors) {
  FEDHeader fedHeader(reinterpret_cast<const unsigned char*>(header));
  if (!fedHeader.check())
    return false;  // throw exception?
  if (fedHeader.sourceID() != fedId) {
    LogDebug("PixelDataFormatter::interpretRawData, fedHeader.sourceID() != fedId")
        << ", sourceID = " << fedHeader.sourceID() << ", fedId = " << fedId << ", errorType = 32";
    errorsInEvent = true;
    if (includeErrors) {
      int errorType = 32;
      SiPixelRawDataError error(*header, errorType, fedId);
      errors[dummyDetId].push_back(error);
    }
  }
  return fedHeader.moreHeaders();
}

bool ErrorCheckerBase::checkTrailer(
    bool& errorsInEvent, int fedId, unsigned int nWords, const Word64* trailer, Errors& errors) {
  FEDTrailer fedTrailer(reinterpret_cast<const unsigned char*>(trailer));
  if (!fedTrailer.check()) {
    if (includeErrors) {
      int errorType = 33;
      SiPixelRawDataError error(*trailer, errorType, fedId);
      errors[dummyDetId].push_back(error);
    }
    errorsInEvent = true;
    LogError("FedTrailerCheck") << "fedTrailer.check failed, Fed: " << fedId << ", errorType = 33";
    return false;
  }
  if (fedTrailer.fragmentLength() != nWords) {
    LogError("FedTrailerLenght") << "fedTrailer.fragmentLength()!= nWords !! Fed: " << fedId << ", errorType = 34";
    errorsInEvent = true;
    if (includeErrors) {
      int errorType = 34;
      SiPixelRawDataError error(*trailer, errorType, fedId);
      errors[dummyDetId].push_back(error);
    }
  }
  return fedTrailer.moreTrailers();
}

void ErrorCheckerBase::conversionError(
    int fedId, const SiPixelFrameConverter* converter, int status, Word32& errorWord, Errors& errors) {
  switch (status) {
    case (1): {
      LogDebug("ErrorChecker::conversionError") << " Fed: " << fedId << "  invalid channel Id (errorType=35)";
      if (includeErrors) {
        int errorType = 35;
        SiPixelRawDataError error(errorWord, errorType, fedId);
        cms_uint32_t detId = errorDetId(converter, errorType, errorWord);
        errors[detId].push_back(error);
      }
      break;
    }
    case (2): {
      LogDebug("ErrorChecker::conversionError") << " Fed: " << fedId << "  invalid ROC Id (errorType=36)";
      if (includeErrors) {
        int errorType = 36;
        SiPixelRawDataError error(errorWord, errorType, fedId);
        cms_uint32_t detId = errorDetId(converter, errorType, errorWord);
        errors[detId].push_back(error);
      }
      break;
    }
    case (3): {
      LogDebug("ErrorChecker::conversionError") << " Fed: " << fedId << "  invalid dcol/pixel value (errorType=37)";
      if (includeErrors) {
        int errorType = 37;
        SiPixelRawDataError error(errorWord, errorType, fedId);
        cms_uint32_t detId = errorDetId(converter, errorType, errorWord);
        errors[detId].push_back(error);
      }
      break;
    }
    case (4): {
      LogDebug("ErrorChecker::conversionError") << " Fed: " << fedId << "  dcol/pixel read out of order (errorType=38)";
      if (includeErrors) {
        int errorType = 38;
        SiPixelRawDataError error(errorWord, errorType, fedId);
        cms_uint32_t detId = errorDetId(converter, errorType, errorWord);
        errors[detId].push_back(error);
      }
      break;
    }
    default:
      LogDebug("ErrorChecker::conversionError") << "  cabling check returned unexpected result, status = " << status;
  };
}
