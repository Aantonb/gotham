/*! \file CudaShadingContext_cuda.cu
 *  \author Jared Hoberock
 *  \brief CUDA implementation of CudaShadingContext class.
 */

// XXX remove this when exceptions are enabled in nvcc
#define BOOST_NO_EXCEPTIONS

#include <stdcuda/stride_cast.h>
#include "CudaShadingContext.h"

#include "CudaScatteringDistributionFunction.h"

using namespace stdcuda;

struct Parameters
{
  const CudaScatteringDistributionFunction *f;
  const CudaDifferentialGeometry *dg;
  int dgStride;
  const float3 *u;
  int uStride;
  float3 *s;
  int sStride;
  float3 *wo;
  int woStride;
  float *pdf;
  int pdfStride;
  bool *delta;
  int deltaStride;
}; // end Parameters

void __global__ sampleUnidirectionalKernel(const Parameters p)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;

  // unpack the streams (don't cross)
  // XXX this shit doesn't compile, thanks cuda
  //const CudaDifferentialGeometry *diffG = stride_cast(p.dg, i, p.dgStride);
  //float3 x = *stdcuda::stride_cast<float3>(u, i, uStride);
  //float3 &result = *stride_cast(wo, i, woStride);
  //float &pdfResult = *stride_cast(pdf, i, pdfStride);
  //bool &deltaResult = *stride_cast(delta, i, deltaStride);

  const char *ptr = reinterpret_cast<const char*>(p.dg) + i*p.dgStride;
  const CudaDifferentialGeometry &dg = *reinterpret_cast<const CudaDifferentialGeometry*>(ptr);

  ptr = reinterpret_cast<const char*>(p.u) + i*p.uStride;
  float3 u = *reinterpret_cast<const float3*>(ptr);

  char *temp = reinterpret_cast<char*>(p.s) + i*p.sStride;
  float3 &s = *reinterpret_cast<float3*>(temp);

  temp = reinterpret_cast<char*>(p.wo) + i*p.woStride;
  float3 &wo = *reinterpret_cast<float3*>(temp);

  temp = reinterpret_cast<char*>(p.pdf) + i*p.pdfStride;
  float &pdf = *reinterpret_cast<float*>(temp);

  temp = reinterpret_cast<char*>(p.delta) + i*p.deltaStride;
  bool &delta = *reinterpret_cast<bool*>(temp);

  const CudaScatteringDistributionFunction &f = p.f[i];

  // sample
  f.sample(dg, u.x, u.y, u.z, s, wo, pdf, delta);
} // end sampleUnidirectionalKernel()

void CudaShadingContext
  ::sampleUnidirectionalScattering(const device_ptr<const CudaScatteringDistributionFunction> &f,
                                   const device_ptr<const CudaDifferentialGeometry> &dg,
                                   const size_t dgStride,
                                   const device_ptr<const float3> &u,
                                   const size_t uStride,
                                   const device_ptr<float3> &s,
                                   const size_t sStride,
                                   const device_ptr<float3> &wo,
                                   const size_t woStride,
                                   const device_ptr<float> &pdf,
                                   const size_t pdfStride,
                                   const device_ptr<bool> &delta,
                                   const size_t deltaStride,
                                   const size_t n)
{
  unsigned int BLOCK_SIZE = 192;
  unsigned int gridSize = n / BLOCK_SIZE;

  Parameters p = {f, dg, dgStride, u, uStride, s, sStride, wo, woStride, pdf, pdfStride, delta, deltaStride};

  if(gridSize)
    sampleUnidirectionalKernel<<<gridSize,BLOCK_SIZE>>>(p);
  if(n%BLOCK_SIZE)
  {
    Parameters p = {f + gridSize*BLOCK_SIZE,
                    stride_cast(dg.get(), gridSize*BLOCK_SIZE, dgStride),
                    dgStride,
                    stride_cast(u.get(),  gridSize*BLOCK_SIZE, uStride),
                    uStride,
                    stride_cast(s.get(),  gridSize*BLOCK_SIZE, sStride),
                    sStride,
                    stride_cast(wo.get(), gridSize*BLOCK_SIZE, woStride),
                    woStride,
                    stride_cast(pdf.get(), gridSize*BLOCK_SIZE, pdfStride),
                    pdfStride,
                    stride_cast(delta.get(), gridSize*BLOCK_SIZE, deltaStride),
                    deltaStride};
    sampleUnidirectionalKernel<<<1,n%BLOCK_SIZE>>>(p);
  } // end if
} // end CudaDifferentialGeometry::sampleUnidirectionalScattering()

