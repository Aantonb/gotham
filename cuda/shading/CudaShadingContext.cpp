/*! \file CudaShadingContext.cpp
 *  \author Jared Hoberock
 *  \brief Implementation of CudaShadingContext class.
 */

#include "CudaShadingContext.h"
#include "CudaScatteringDistributionFunction.h"
#include "CudaLambertian.h"
#include "CudaHemisphericalEmission.h"
#include "../../shading/Lambertian.h"
#include "../../shading/HemisphericalEmission.h"
#include "evaluateBidirectionalScattering.h"
#include "evaluateUnidirectionalScattering.h"
#include <stdcuda/cuda_algorithm.h>

CudaShadingContext
  ::CudaShadingContext(const boost::shared_ptr<MaterialList> &materials)
    :Parent(materials)
{
  ;
} // end CudaShadingContext::CudaShadingContext()

void CudaShadingContext
  ::evaluateBidirectionalScattering(ScatteringDistributionFunction **f,
                                    const Vector *wo,
                                    const DifferentialGeometry *dg,
                                    const Vector *wi,
                                    const int *stencil,
                                    Spectrum *results,
                                    const size_t n)
{
  // create CUDA-compatible bsdfs
  stdcuda::vector_dev<CudaScatteringDistributionFunction> cf;
  createCudaScatteringDistributionFunctions(f, stencil, n, cf);

  // create device scratch space
  stdcuda::vector_dev<float3> woDevice(n);
  stdcuda::vector_dev<float3> wiDevice(n);
  stdcuda::vector_dev<CudaDifferentialGeometry> dgDevice(n);
  stdcuda::vector_dev<int> stencilDevice(n);
  stdcuda::copy(wo, wo + n, woDevice.begin());
  stdcuda::copy(wi, wi + n, wiDevice.begin());
  stdcuda::copy(dg, dg + n, dgDevice.begin());
  stdcuda::copy(stencil, stencil + n, stencilDevice.begin());

  stdcuda::vector_dev<Spectrum> resultsDevice(n);
  ::evaluateBidirectionalScattering(&cf[0],
                                    &woDevice[0],
                                    &dgDevice[0],
                                    &wiDevice[0],
                                    &stencilDevice[0],
                                    &resultsDevice[0],
                                    n);

  // copy results back to host
  stdcuda::copy(resultsDevice.begin(), resultsDevice.end(), &results[0]);
} // end CudaShadingContext::evaluateBidirectionalScattering()

void CudaShadingContext
  ::evaluateUnidirectionalScattering(ScatteringDistributionFunction **f,
                                     const Vector *wo,
                                     const DifferentialGeometry *dg,
                                     const int *stencil,
                                     Spectrum *results,
                                     const size_t n)
{
  // create CUDA-compatible bsdfs
  stdcuda::vector_dev<CudaScatteringDistributionFunction> cf;
  createCudaScatteringDistributionFunctions(f, stencil, n, cf);

  // create device scratch space
  stdcuda::vector_dev<float3> woDevice(n);
  stdcuda::vector_dev<CudaDifferentialGeometry> dgDevice(n);
  stdcuda::vector_dev<int> stencilDevice(n);
  stdcuda::copy(wo, wo + n, woDevice.begin());
  stdcuda::copy(dg, dg + n, dgDevice.begin());
  stdcuda::copy(stencil, stencil + n, stencilDevice.begin());

  stdcuda::vector_dev<Spectrum> resultsDevice(n);
  ::evaluateUnidirectionalScattering(&cf[0],
                                     &woDevice[0],
                                     &dgDevice[0],
                                     &stencilDevice[0],
                                     &resultsDevice[0],
                                     n);

  // copy results back to host
  stdcuda::copy(resultsDevice.begin(), resultsDevice.end(), &results[0]);
} // end CudaShadingContext::evaluateUnidirectionalScattering()

void CudaShadingContext
  ::createCudaScatteringDistributionFunctions(ScatteringDistributionFunction **f,
                                              const int *stencil,
                                              const size_t n,
                                              stdcuda::vector_dev<CudaScatteringDistributionFunction> &cf)
{
  cf.resize(n);
  for(size_t i = 0; i != n; ++i)
  {
    if(stencil[i])
    {
      cf[i] = createCudaScatteringDistributionFunction(f[i]);
    } // end if
  } // end for i
} // end CudaScatteringDistributionFunctions::createCudaScatteringDistributionFunctions()

CudaScatteringDistributionFunction CudaShadingContext
  ::createCudaScatteringDistributionFunction(const ScatteringDistributionFunction *f)
{
  CudaScatteringDistributionFunction result;

  // switch on the type of f
  if(dynamic_cast<const Lambertian*>(f) != 0)
  {
    const Lambertian *l = static_cast<const Lambertian*>(f);
    CudaLambertian *cl = reinterpret_cast<CudaLambertian*>(&result.mFunction);

    *cl = CudaLambertian(l->getAlbedo());

    result.mType = LAMBERTIAN;
  } // end if
  else if(dynamic_cast<const HemisphericalEmission*>(f) != 0)
  {
    const HemisphericalEmission *he = static_cast<const HemisphericalEmission*>(f);
    CudaHemisphericalEmission *che = reinterpret_cast<CudaHemisphericalEmission*>(&result.mFunction);

    *che = CudaHemisphericalEmission(he->getRadiance());

    result.mType = HEMISPHERICAL_EMISSION;
  } // end else if

  return result;
} // end CudaShadingContext::createCudaScatteringDistributionFunction()

