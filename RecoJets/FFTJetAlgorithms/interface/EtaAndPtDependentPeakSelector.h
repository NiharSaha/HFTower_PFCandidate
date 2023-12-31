#ifndef RecoJets_FFTJetAlgorithm_EtaAndPtDependentPeakSelector_h
#define RecoJets_FFTJetAlgorithm_EtaAndPtDependentPeakSelector_h

#include "fftjet/Peak.hh"
#include "fftjet/SimpleFunctors.hh"
#include "fftjet/LinearInterpolator2d.hh"

#include "RecoJets/FFTJetAlgorithms/interface/LookupTable2d.h"

namespace fftjetcms {
  //
  // Interpolation is linear in eta, log(scale), and log(magnitude).
  // It is assumed that the first variable in the table is eta and
  // the second is log(scale). It is assumed that the table value
  // is log(magnitude).
  //
  class EtaAndPtDependentPeakSelector : public fftjet::Functor1<bool, fftjet::Peak> {
  public:
    explicit EtaAndPtDependentPeakSelector(std::istream& in);
    EtaAndPtDependentPeakSelector() = delete;
    EtaAndPtDependentPeakSelector(const EtaAndPtDependentPeakSelector&) = delete;
    EtaAndPtDependentPeakSelector& operator=(const EtaAndPtDependentPeakSelector&) = delete;
    ~EtaAndPtDependentPeakSelector() override;

    bool operator()(const fftjet::Peak& peak) const override;
    inline bool isValid() const { return ip_; }

  private:
    fftjet::LinearInterpolator2d* ip_;
  };

  // Similar class which does not perform linear interpolation but
  // simply looks up in a histogram-like table
  class EtaAndPtLookupPeakSelector : public fftjet::Functor1<bool, fftjet::Peak> {
  public:
    EtaAndPtLookupPeakSelector(
        unsigned nx, double xmin, double xmax, unsigned ny, double ymin, double ymax, const std::vector<double>& data);

    bool operator()(const fftjet::Peak& peak) const override;

  private:
    LookupTable2d lookupTable_;
  };
}  // namespace fftjetcms

#endif  // RecoJets_FFTJetAlgorithm_EtaAndPtDependentPeakSelector_h
