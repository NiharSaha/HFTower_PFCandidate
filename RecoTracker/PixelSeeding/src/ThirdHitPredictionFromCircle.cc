#include <cmath>
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "RecoTracker/TkMSParametrization/interface/PixelRecoUtilities.h"
#include "RecoTracker/TkMSParametrization/interface/PixelRecoRange.h"

#include "RecoTracker/PixelSeeding/interface/ThirdHitPredictionFromCircle.h"
#include "FWCore/Utilities/interface/Likely.h"

#include <iostream>
#include <algorithm>

#include "DataFormats/Math/interface/normalizedPhi.h"
#include "DataFormats/Math/interface/approx_asin.h"

// there are tons of safety checks.
// Try to move all of the out the regular control flow using gcc magic
#include "DataFormats/Math/interface/approx_atan2.h"
namespace {
  inline float f_atan2f(float y, float x) { return unsafe_atan2f<7>(y, x); }
  template <typename V>
  inline float f_phi(V v) {
    return f_atan2f(v.y(), v.x());
  }
}  // namespace

namespace {
  template <class T>
  inline T sqr(T t) {
    return t * t;
  }
  template <class T>
  inline T sgn(T t) {
    return std::signbit(t) ? -T(1.) : T(1.);
  }
  template <class T>
  inline T clamped_acos(T x) {
    x = std::min(T(1.), std::max(-T(1.), x));
    return unsafe_acos<5>(x);
  }

  template <class T>
  inline T clamped_sqrt(T t) {
    return std::sqrt(std::max(T(0), t));
  }
}  // namespace

ThirdHitPredictionFromCircle::ThirdHitPredictionFromCircle(const GlobalPoint &P1,
                                                           const GlobalPoint &P2,
                                                           float tolerance)
    : p1(P1.x(), P1.y()), theTolerance(tolerance) {
  Vector2D p2(P2.x(), P2.y());
  Vector2D diff = 0.5 * (p2 - p1);
  delta2 = diff.mag2();
  delta = std::sqrt(delta2);
  axis = Vector2D(-diff.y(), diff.x()) / delta;
  center = p1 + diff;
}

float ThirdHitPredictionFromCircle::phi(float curvature, float radius) const {
  float phi;
  if (UNLIKELY(std::abs(curvature) < float(1.0e-5))) {
    float cos = (center * axis) / radius;
    phi = axis.phi() - clamped_acos(cos);
  } else {
    float sign = sgn(curvature);
    float radius2 = sqr(1.0f / curvature);
    float orthog = clamped_sqrt(radius2 - delta2);
    Basic2DVector<float> lcenter = Basic2DVector<float>(center) - sign * orthog * Basic2DVector<float>(axis);
    float rc2 = lcenter.mag2();
    float r2 = sqr(radius);
    float cos = (rc2 + r2 - radius2) / std::sqrt(4.f * rc2 * r2);
    phi = f_phi(lcenter) + sign * clamped_acos(cos);
  }
  return phi;  // not normalized
}

float ThirdHitPredictionFromCircle::angle(float curvature, float radius) const {
  if (UNLIKELY(std::abs(curvature) < float(1.0e-5))) {
    float sin = (center * axis) / radius;
    return sin / clamped_sqrt(1 - sqr(sin));
  } else {
    float radius2 = sqr(1.0f / curvature);
    float orthog = clamped_sqrt(radius2 - delta2);
    Basic2DVector<float> lcenter = Basic2DVector<float>(center) - sgn(curvature) * orthog * Basic2DVector<float>(axis);

    float cos = (radius2 + sqr(radius) - lcenter.mag2()) * curvature / (2 * radius);
    return -cos / clamped_sqrt(1.f - sqr(cos));
  }
}

ThirdHitPredictionFromCircle::Range ThirdHitPredictionFromCircle::operator()(Range curvature, float radius) const {
  float phi1 = normalizedPhi(phi(curvature.second, radius));
  float phi2 = proxim(phi(curvature.first, radius), phi1);
  if (phi2 < phi1)
    std::swap(phi2, phi1);

  return Range(phi1 * radius - theTolerance, phi2 * radius + theTolerance);
}

ThirdHitPredictionFromCircle::Range ThirdHitPredictionFromCircle::curvature(double transverseIP) const {
  // this is a mess.  Use a CAS and lots of drawings to verify...

  transverseIP = std::abs(transverseIP);
  double transverseIP2 = sqr(transverseIP);
  double tip = axis * center;
  double tip2 = sqr(tip);
  double lip = axis.x() * center.y() - axis.y() * center.x();
  double lip2 = sqr(lip);

  double origin = std::sqrt(tip2 + lip2);
  double tmp1 = lip2 + tip2 - transverseIP2;
  double tmp2 = 2. * (tip - transverseIP) * (tip + transverseIP);
  double tmp3 = 2. * delta * origin;
  double tmp4 = tmp1 + delta2;
  double tmp5 = 2. * delta * lip;

  // I am probably being overly careful here with border cases
  // but you never know what crap you might get fed
  // VI fixed for finiteMath

  double u1 = 0, u2 = 0;
  constexpr double SMALL = 1.0e-23;
  constexpr double LARGE = 1.0e23;

  if (UNLIKELY(tmp4 - tmp5 < 1.0e-15)) {
    u1 = -SMALL;
    u2 = +SMALL;
  } else {
    if (UNLIKELY(std::abs(tmp2) < 1.0e-15)) {
      // the denominator is zero
      // this means that one of the tracks will be straight
      // and the other can be computed from the limit of the equation
      double tmp = lip2 - delta2;
      u1 = LARGE;
      u2 = (sqr(0.5 * tmp) - delta2 * tip2) / (tmp * tip);
      if (tip < 0)
        std::swap(u1, u2);
    } else {
      double tmp6 = (tmp4 - tmp5) * (tmp4 + tmp5);
      if (UNLIKELY(tmp6 < 1.0e-15)) {
        u1 = -SMALL;
        u2 = +SMALL;
      } else {
        double tmp7 = tmp6 > 0 ? (transverseIP * std::sqrt(tmp6) / tmp2) : 0.;
        double tmp8 = tip * (tmp1 - delta2) / tmp2;
        // the two quadratic solutions
        u1 = tmp8 + tmp7;
        u2 = tmp8 - tmp7;
      }
    }

    if (tmp4 <= std::abs(tmp3)) {
      if ((tmp3 < 0) == (tip < 0))
        u2 = +SMALL;
      else
        u1 = -SMALL;
    }
  }

  return Range(sgn(u1) / std::sqrt(sqr(u1) + delta2), sgn(u2) / std::sqrt(sqr(u2) + delta2));
}

ThirdHitPredictionFromCircle::Scalar ThirdHitPredictionFromCircle::invCenterOnAxis(const Vector2D &p2) const {
  Vector2D del = p2 - p1;
  Vector2D axis2 = Vector2D(-del.y(), del.x()) / del.mag();
  Vector2D diff = p1 + 0.5f * del - center;
  Scalar a = diff.y() * axis2.x() - diff.x() * axis2.y();
  Scalar b = axis.y() * axis2.x() - axis.x() * axis2.y();
  return b / a;
}

double ThirdHitPredictionFromCircle::curvature(const Vector2D &p2) const {
  double invDist = invCenterOnAxis(p2);
  double invDist2 = sqr(invDist);
  double curv = std::sqrt(invDist2 / (1. + invDist2 * delta2));
  return sgn(invDist) * curv;
}

double ThirdHitPredictionFromCircle::transverseIP(const Vector2D &p2) const {
  double invDist = invCenterOnAxis(p2);
  if (UNLIKELY(std::abs(invDist) < 1.0e-5))
    return std::abs(p2 * axis);
  else {
    double dist = 1.0 / invDist;
    double radius = std::sqrt(sqr(dist) + delta2);
    return std::abs((center + axis * dist).mag() - radius);
  }
}

//------------------------------------------------------------------------------

namespace {
  // 2asin(cd)/c
  inline float arc(float c, float d) {
    float z = c * d;
    z *= z;
    float x = 2.f * d;

    if (z > 0.5f)
      return std::min(1.f, 2.f * unsafe_asin<5>(c * d) / c);

    return x * approx_asin_P<7>(z);
  }
}  // namespace

ThirdHitPredictionFromCircle::HelixRZ::HelixRZ(const ThirdHitPredictionFromCircle *icircle,
                                               double iz1,
                                               double z2,
                                               double curv)
    : circle(icircle), curvature(curv), radius(1. / curv), z1(iz1) {
  Scalar orthog = sgn(curv) * clamped_sqrt(radius * radius - circle->delta2);
  center = circle->center + orthog * circle->axis;

  Scalar absCurv = std::abs(curv);
  seg = circle->delta;
  seg = arc(absCurv, seg);
  dzdu = (z2 - z1) / seg;
}

double ThirdHitPredictionFromCircle::HelixRZ::maxCurvature(const ThirdHitPredictionFromCircle *circle,
                                                           double z1,
                                                           double z2,
                                                           double z3) {
  constexpr double maxAngle = M_PI;
  double halfAngle = (0.5 * maxAngle) * (z2 - z1) / (z3 - z1);
  if (UNLIKELY(halfAngle <= 0.0))
    return 0.0;

  return std::sin(halfAngle) / circle->delta;
}

ThirdHitPredictionFromCircle::HelixRZ::Scalar ThirdHitPredictionFromCircle::HelixRZ::zAtR(Scalar r) const {
  if (UNLIKELY(std::abs(curvature) < 1.0e-5)) {
    Scalar tip = circle->axis * circle->p1;
    Scalar lip = circle->axis.y() * circle->p1.x() - circle->axis.x() * circle->p1.y();
    return z1 + (std::sqrt(sqr(r) - sqr(tip)) - lip) * dzdu;
  }

  Scalar radius2 = sqr(radius);

  Scalar b2 = center.mag2();
  Scalar b = std::sqrt(b2);

  Scalar cos1 = 0.5 * curvature * (radius2 + b2 - sqr(r)) / b;
  Scalar cos2 = 0.5 * curvature * (radius2 + b2 - circle->p1.mag2()) / b;

  Scalar phi1 = clamped_acos(cos1);
  Scalar phi2 = clamped_acos(cos2);

  // more plausbility checks needed...
  // the two circles can have two possible intersections
  Scalar u1 = std::abs((phi1 - phi2) * radius);
  Scalar u2 = std::abs((phi1 + phi2) * radius);

  return z1 + ((u1 >= seg && u1 < u2) ? u1 : u2) * dzdu;
}

#include <vdt/sincos.h>

ThirdHitPredictionFromCircle::HelixRZ::Scalar ThirdHitPredictionFromCircle::HelixRZ::rAtZ(Scalar z) const {
  if (UNLIKELY(std::abs(dzdu) < 1.0e-5))
    return 99999.0;

  if (UNLIKELY(std::abs(curvature) < 1.0e-5)) {
    Scalar tip = circle->axis * circle->p1;
    Scalar lip = circle->axis.y() * circle->p1.x() - circle->axis.x() * circle->p1.y();
    return std::sqrt(sqr(tip) + sqr(lip + (z - z1) / dzdu));
  }

  // we won't go below that (see comment below)
  Scalar minR2 = (2. * circle->center - circle->p1).mag2();

  float phi = curvature * (z - z1) / dzdu;

  if (UNLIKELY(std::abs(phi) > 2. * M_PI)) {
    // with a helix we can get problems here - this is used to get the limits
    // however, if phi gets large, we get into the regions where we loop back
    // to smaller r's.  The question is - do we care about these tracks?
    // The answer is probably no:  Too much pain, and the rest of the
    // tracking won't handle looping tracks anyway.
    // So, what we do here is to return nothing smaller than the radius
    // than any of the two hits, i.e. the second hit, which is presumably
    // outside of the 1st hit.

    return std::sqrt(minR2);
  }

  Vector2D rel = circle->p1 - center;

  float c;
  float s;
  vdt::fast_sincosf(phi, s, c);

  Vector2D p(center.x() + c * rel.x() - s * rel.y(), center.y() + s * rel.x() + c * rel.y());

  return std::sqrt(std::max(minR2, p.mag2()));
}
