#ifndef CommonTools_Utils_MethodArgumentSetter_h
#define CommonTools_Utils_MethodArgumentSetter_h
/* \class reco::parser::MethodArgumentSetter
 *
 * MethodArgumenteger setter
 *
 * \author  Luca Lista, INFN
 *
 * \version $Revision: 1.1 $
 *
 */
#include "CommonTools/Utils/interface/MethodArgumentStack.h"
#include <cassert>

namespace reco {
  namespace parser {
    struct MethodArgumentSetter {
      MethodArgumentSetter(MethodArgumentStack &stack) : stack_(stack) {}
      template <typename T>
      void operator()(const T &n) const {
        stack_.push_back(AnyMethodArgument(n));
      }
      void operator()(const char *begin, const char *end) const {
        assert(begin + 1 <= end - 1);  // the quotes are included in [begin,end[ range.
        //in boost 1.67, the parser appends any extra white space to the
        // end
        if (*(end - 1) != *begin) {
          --end;
        }
        stack_.push_back(AnyMethodArgument(std::string(begin + 1, end - 1)));
      }

    private:
      MethodArgumentStack &stack_;
    };
  }  // namespace parser
}  // namespace reco

#endif
