#ifndef CommonTools_Utils_formulaParameterEvaluator_h
#define CommonTools_Utils_formulaParameterEvaluator_h
// -*- C++ -*-
//
// Package:     CommonTools/Utils
// Class  :     reco::formula::ParameterEvaluator
//
/**\class reco::formula::ParameterEvaluator formulaParameterEvaluator.h "formulaParameterEvaluator.h"

 Description: [one line class summary]

 Usage:
    <usage>

*/
//
// Original Author:  Christopher Jones
//         Created:  Wed, 23 Sep 2015 18:06:27 GMT
//

// system include files

// user include files
#include "formulaEvaluatorBase.h"

// forward declarations

namespace reco {
  namespace formula {
    class ParameterEvaluator : public EvaluatorBase {
    public:
      explicit ParameterEvaluator(unsigned int iIndex) : m_index(iIndex) {}

      // ---------- const member functions ---------------------
      double evaluate(double const* iVariables, double const* iParameters) const final;
      std::vector<std::string> abstractSyntaxTree() const final;

      ParameterEvaluator(const ParameterEvaluator&) = delete;

      const ParameterEvaluator& operator=(const ParameterEvaluator&) = delete;

    private:
      // ---------- member data --------------------------------
      unsigned int m_index;
    };
  }  // namespace formula
}  // namespace reco

#endif
