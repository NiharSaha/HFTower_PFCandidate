#ifndef CommonTools_Statistics_median_h
#define CommonTools_Statistics_median_h

#include <vector>
#include <algorithm>
#include <cmath>
//#include "CommonDet/DetUtilities/interface/DetExceptions.h"

//using namespace std;

/**
 *  Median in one dimension.
 *  T must be sortable, 'addable', and dividable by 2.
 */
template <class T>
T median(std::vector<T> values) {
  switch (values.size()) {
    case 0:
      //      throw LogicError ("Median of empty vector");
    case 1:
      return values[0];
    case 2:
      return (values[0] + values[1]) / 2.;
  };
  T ret;
  const int size = values.size();
  const int half = (int)ceil(size / 2.);
  partial_sort(values.begin(), values.begin() + half + 1, values.end());
  if (size & 1) {
    ret = values[half - 1];
  } else {
    ret = (values[half - 1] + values[half]) / 2.;
  };
  return ret;
}
#endif
