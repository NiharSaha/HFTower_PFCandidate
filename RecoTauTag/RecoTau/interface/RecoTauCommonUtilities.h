#ifndef RecoTauTag_RecoTau_RecoTauCommonUtilities_h
#define RecoTauTag_RecoTau_RecoTauCommonUtilities_h

/*
 * RecoTauCommonUtilities - utilities for extracting PFCandidates from
 * PFJets and summing over collections of PFCandidates.
 *
 * Author: Evan K. Friis (UC Davis)
 *
 */

#include <vector>
#include <algorithm>
#include <numeric>

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// Boost helpers
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/mem_fn.hpp>

#include <boost/type_traits/is_base_of.hpp>

namespace reco {
  namespace tau {

    class SortPFCandsDescendingPt {
    public:
      bool operator()(const CandidatePtr& a, const CandidatePtr& b) const { return a->pt() > b->pt(); }
    };

    /// Filter a collection of objects that are convertible to CandidatePtrs
    /// by PFCandidate ID
    template <typename Iterator>
    std::vector<CandidatePtr> filterPFCandidates(const Iterator& begin,
                                                 const Iterator& end,
                                                 int pdgId,
                                                 bool sort = true) {
      std::vector<CandidatePtr> output;
      for (Iterator iter = begin; iter != end; ++iter) {
        const reco::CandidatePtr& ptr(*iter);
        if (std::abs(ptr->pdgId()) == pdgId)
          output.push_back(ptr);
      }
      if (sort)
        std::sort(output.begin(), output.end(), SortPFCandsDescendingPt());
      return output;
    }

    /// Extract pfCandidates of a given particle Id from a PFJet.  If sort is true,
    /// candidates will be sorted by descending PT. Internally translates to pdgId
    std::vector<CandidatePtr> pfCandidates(const Jet& jet, int particleId, bool sort = true);

    /// Extract pfCandidates of a that match a list of particle Ids from a PFJet
    std::vector<CandidatePtr> pfCandidates(const Jet& jet, const std::vector<int>& particleIds, bool sort = true);

    /// Extract pfCandidates of a given PDG Id from a PFJet.  If sort is true,
    /// candidates will be sorted by descending PT
    std::vector<CandidatePtr> pfCandidatesByPdgId(const Jet& jet, int pdgId, bool sort = true);

    /// Extract pfCandidates of a that match a list of PDG Ids from a PFJet
    std::vector<CandidatePtr> pfCandidatesByPdgId(const Jet& jet, const std::vector<int>& pdgIds, bool sort = true);

    /// Extract all non-neutral candidates from a PFJet
    std::vector<CandidatePtr> pfChargedCands(const Jet& jet, bool sort = true);

    /// Extract all pfGammas from a PFJet
    std::vector<CandidatePtr> pfGammas(const Jet& jet, bool sort = true);

    /// Flatten a list of pi zeros into a list of there constituent PFCandidates
    std::vector<CandidatePtr> flattenPiZeros(const std::vector<RecoTauPiZero>::const_iterator&,
                                             const std::vector<RecoTauPiZero>::const_iterator&);
    std::vector<CandidatePtr> flattenPiZeros(const std::vector<RecoTauPiZero>&);

    /// Convert a BaseView (View<T>) to a TRefVector
    template <typename RefVectorType, typename BaseView>
    RefVectorType castView(const edm::Handle<BaseView>& view) {
      typedef typename RefVectorType::value_type OutputRef;
      // Double check at compile time that the inheritance is okay.  It can still
      // fail at runtime if you pass it the wrong collection.
      static_assert((boost::is_base_of<typename BaseView::value_type, typename RefVectorType::member_type>::value));
      RefVectorType output;
      size_t nElements = view->size();
      output.reserve(nElements);
      // Cast each of our Refs
      for (size_t i = 0; i < nElements; ++i) {
        output.push_back(view->refAt(i).template castTo<OutputRef>());
      }
      return output;
    }

    /*
 *Given a range over a container of type C, return a new 'end' iterator such
 *that at max <N> elements are taken.  If there are less than N elements in the
 *array, leave the <end>  as it is
 */
    template <typename InputIterator>
    InputIterator takeNElements(const InputIterator& begin, const InputIterator& end, size_t N) {
      size_t input_size = end - begin;
      return (N > input_size) ? end : begin + N;
    }

    /// Sum the four vectors in a collection of PFCandidates
    template <typename InputIterator, typename FunctionPtr, typename ReturnType>
    ReturnType sumPFVector(InputIterator begin, InputIterator end, FunctionPtr func, ReturnType init) {
      ReturnType output = init;
      for (InputIterator cand = begin; cand != end; ++cand) {
        //#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
        output += ((**cand).*(func))();
      }
      return output;
    }

    template <typename InputIterator>
    reco::Candidate::LorentzVector sumPFCandP4(InputIterator begin, InputIterator end) {
      return sumPFVector(begin, end, &Candidate::p4, reco::Candidate::LorentzVector());
    }

    /// Sum the pT of a collection of PFCandidates
    template <typename InputIterator>
    double sumPFCandPt(InputIterator begin, InputIterator end) {
      return sumPFVector(begin, end, &Candidate::pt, 0.0);
    }

    /// Sum the charge of a collection of PFCandidates
    template <typename InputIterator>
    int sumPFCandCharge(InputIterator begin, InputIterator end) {
      return sumPFVector(begin, end, &Candidate::charge, 0);
    }

    template <typename InputIterator>
    InputIterator leadCand(InputIterator begin, InputIterator end) {
      double max_pt = 0;
      InputIterator max_cand = begin;
      for (InputIterator cand = begin; cand != end; ++cand) {
        if ((*cand)->pt() > max_pt) {
          max_pt = (*cand)->pt();
          max_cand = cand;
        }
      }
      return max_cand;
    }

    math::XYZPointF atECALEntrance(const reco::Candidate* part, double bField);

  }  // namespace tau
}  // namespace reco
#endif
