#ifndef CommonTools_Utils_formulaEvaluatorBase_h
#define CommonTools_Utils_formulaEvaluatorBase_h
// -*- C++ -*-
//
// Package:     CommonTools/Utils
// Class  :     reco::formula::EvaluatorBase
//
/**\class reco::formula::EvaluatorBase formulaEvaluatorBase.h "formulaEvaluatorBase.h"

 Description: Base class for formula evaluators

 Usage:
    Used as an internal detail on the reco::FormulaEvalutor class. 
    Base class for all objects used in the abstract evaluation tree where one node
    corresponds to one syntax element of the formula.

*/
//
// Original Author:  Christopher Jones
//         Created:  Wed, 23 Sep 2015 16:26:00 GMT
//

// system include files
#include <vector>
#include <string>

// user include files

// forward declarations

namespace reco {
  namespace formula {
    std::vector<std::string> shiftAST(std::vector<std::string> child);
    class EvaluatorBase {
    public:
      enum class Precedence {
        kIdentity = 1,
        kComparison = 2,
        kPlusMinus = 3,
        kMultDiv = 4,
        kPower = 5,
        kFunction = 6,  //default
        kParenthesis = 7,
        kUnaryMinusOperator = 8
      };

      EvaluatorBase();
      EvaluatorBase(Precedence);
      virtual ~EvaluatorBase();

      // ---------- const member functions ---------------------
      //inputs are considered to be 'arrays' which have already been validated to
      // be of the appropriate length
      virtual double evaluate(double const* iVariables, double const* iParameters) const = 0;
      virtual std::vector<std::string> abstractSyntaxTree() const = 0;

      unsigned int precedence() const { return m_precedence; }
      void setPrecedenceToParenthesis() { m_precedence = static_cast<unsigned int>(Precedence::kParenthesis); }

      EvaluatorBase(const EvaluatorBase&) = delete;

      const EvaluatorBase& operator=(const EvaluatorBase&) = delete;

    private:
      // ---------- member data --------------------------------
      unsigned int m_precedence;
    };
  }  // namespace formula
}  // namespace reco

#endif
