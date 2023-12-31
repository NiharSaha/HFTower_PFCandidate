#ifndef CommonTools_Utils_formulaConstantEvaluator_h
#define CommonTools_Utils_formulaConstantEvaluator_h
// -*- C++ -*-
//
// Package:     CommonTools/Utils
// Class  :     reco::formula::ConstantEvaluator
//
/**\class reco::formula::ConstantEvaluator formulaConstantEvaluator.h "formulaConstantEvaluator.h"

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
    class ConstantEvaluator : public EvaluatorBase {
    public:
      explicit ConstantEvaluator(double iValue) : m_value(iValue) {}

      // ---------- const member functions ---------------------
      double evaluate(double const* iVariables, double const* iParameters) const final;
      std::vector<std::string> abstractSyntaxTree() const final;

      ConstantEvaluator(const ConstantEvaluator&) = delete;

      const ConstantEvaluator& operator=(const ConstantEvaluator&) = delete;

    private:
      // ---------- member data --------------------------------
      double m_value;
    };
  }  // namespace formula
}  // namespace reco

#endif
