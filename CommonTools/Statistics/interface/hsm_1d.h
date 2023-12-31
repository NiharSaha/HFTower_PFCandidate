#ifndef CommonTools_Statistics_hsm_1d_h
#define CommonTools_Statistics_hsm_1d_h

#include "CommonTools/Statistics/interface/StatisticsException.h"

#include <vector>
#include <cmath>
#include <algorithm>

template <class T>
std::pair<typename std::vector<T>::iterator, typename std::vector<T>::iterator> hsm_1d_returning_iterators(
    std::vector<T>& values) {
  sort(values.begin(), values.end());

  typename std::vector<T>::iterator begin = values.begin();
  typename std::vector<T>::iterator end = values.end() - 1;

  while (true) {
    const int size = (int)(end - begin);
    // const int stepsize = size / 2;
    int stepsize = static_cast<int>(floor(.4999999999 + size * .5));
    if (stepsize == 0)
      stepsize = 1;

    end = begin + stepsize;
    T div = (T)fabs((T)(*end) - (T)(*begin));
    typename std::vector<T>::iterator new_begin = begin;
    typename std::vector<T>::iterator new_end = end;

    for (typename std::vector<T>::iterator i = (begin + 1); i != (begin + size - stepsize + 1); ++i) {
      if (fabs((*i) - (*(i + stepsize))) < div) {
        new_begin = i;
        new_end = i + stepsize;
        div = (T)((T)(*new_end) - (T)(*new_begin));
      };
    };
    begin = new_begin;
    end = new_end;
    if (size < 4)
      break;
  };

  std::pair<typename std::vector<T>::iterator, typename std::vector<T>::iterator> ret(begin, end);
  return ret;
}

/**
 *  stepsize Sample Mode one dimension.
 *  T must be sortable, 'addable', and dividable by 2.
 */

template <class T>
T hsm_1d(std::vector<T> values) {
  switch (values.size()) {
    case 0:
      throw StatisticsException("Hsm of empty vector undefined");
    case 1:
      return (T) * (values.begin());
    case 2:
      return (T)((*(values.begin()) + *(values.end() - 1)) / 2);
  };
  std::pair<typename std::vector<T>::iterator, typename std::vector<T>::iterator> itors =
      hsm_1d_returning_iterators(values);
  T ret = ((T)(*(itors.first)) + (T)(*(itors.second))) / (T)2.;
  return ret;
}

#endif
