/*! \file Lambertian.cpp
 *  \author Jared Hoberock
 *  \brief Implementation of Lambertian class.
 */

#include "Lambertian.h"
#include "../geometry/Mappings.h"

Lambertian
  ::Lambertian(const Spectrum &albedo)
    :Parent0(),Parent1(albedo)
{
  ;
} // end Lambertian::Lambertian()

Spectrum Lambertian
  ::evaluate(const Vector &wo,
             const DifferentialGeometry &dg,
             const Vector &wi) const
{
  return Parent1::evaluate(wo,dg.getNormal(),wi);
} // end Lambertian::evaluate()

Spectrum Lambertian
  ::evaluate(const Vector &wo,
             const DifferentialGeometry &dg,
             const Vector &wi,
             const bool delta,
             const ComponentIndex component,
             float &pdf) const
{
  Spectrum result = Spectrum::black();
  pdf = 0;
  if(areSameHemisphere(wi, dg.getNormal(), wo))
  {
    pdf = Mappings<Vector>::evaluateCosineHemispherePdf(wi, dg.getNormal());
    result = mAlbedoOverPi;
  } // end if

  return result;
} // end Lambertian::evaluate()

