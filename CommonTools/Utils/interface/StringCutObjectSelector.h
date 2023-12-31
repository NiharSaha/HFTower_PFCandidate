#ifndef CommonTools_Utils_StringCutObjectSelector_h
#define CommonTools_Utils_StringCutObjectSelector_h
/* \class StringCutObjectSelector
 *
 * \author Luca Lista, INFN
 *
 * $Id: StringCutObjectSelector.h,v 1.4 2012/06/26 21:09:37 wmtan Exp $
 */
#include "FWCore/Utilities/interface/EDMException.h"
#include "CommonTools/Utils/interface/SelectorPtr.h"
#include "CommonTools/Utils/interface/SelectorBase.h"
#include "CommonTools/Utils/interface/cutParser.h"
#include "FWCore/Reflection/interface/ObjectWithDict.h"

template <typename T, bool DefaultLazyness = false>
struct StringCutObjectSelector {
  StringCutObjectSelector(const std::string &cut, bool lazy = DefaultLazyness) : type_(typeid(T)) {
    if (!reco::parser::cutParser<T>(cut, select_, lazy)) {
      throw edm::Exception(edm::errors::Configuration, "failed to parse \"" + cut + "\"");
    }
  }
  StringCutObjectSelector(const reco::parser::SelectorPtr &select) : select_(select), type_(typeid(T)) {}
  bool operator()(const T &t) const {
    edm::ObjectWithDict o(type_, const_cast<T *>(&t));
    return (*select_)(o);
  }

private:
  reco::parser::SelectorPtr select_;
  edm::TypeWithDict type_;
};

#endif
