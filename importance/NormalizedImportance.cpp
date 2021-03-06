/*! \file NormalizedImportance.cpp
 *  \author Jared Hoberock
 *  \brief Implementation of NormalizedImportance class.
 */

#include "NormalizedImportance.h"
#include "LuminanceImportance.h"
#include <stratifiedsequence/StratifiedSequence.h>
#include "../path/KelemenSampler.h"
#include "../path/Path.h"
#include "../path/PathToImage.h"
#include "../shading/ScatteringDistributionFunction.h"
#include <gpcpu/Vector.h>
#include "../records/RenderFilm.h"
#include "../renderers/MetropolisRenderer.h"
#include "../renderers/HaltCriterion.h"

using namespace boost;
using namespace gpcpu;

void NormalizedImportance
  ::preprocess(const shared_ptr<RandomSequence> &r,
               const shared_ptr<const Scene> &scene,
               const shared_ptr<ShadingContext> &context,
               const shared_ptr<PathMutator> &mutator,
               MetropolisRenderer &renderer)
{
  // render a low resolution estimate using LuminanceImportance
  // XXX DESIGN just render into mEstimate
  shared_ptr<RenderFilm> estimate(new RenderFilm(20,20));
  mEstimate.resize(estimate->getWidth(),estimate->getHeight());
  shared_ptr<LuminanceImportance> luminance(new LuminanceImportance());

  // create a separate renderer temporarily
  MetropolisRenderer tempRenderer(r, mutator, luminance);
  tempRenderer.setScene(scene);
  tempRenderer.setRecord(dynamic_pointer_cast<Record,RenderFilm>(estimate));
  tempRenderer.setShadingContext(context);
  shared_ptr<HaltCriterion> halt(new TargetRayCount(1000000));
  tempRenderer.setHaltCriterion(halt);
  
  // render
  Renderer::ProgressCallback callback;
  tempRenderer.render(callback);

  // copy estimate into mEstimate
  // XXX DESIGN this is unnecessary: just render into mEstimate
  // set each pixel to 1 over itself
  for(unsigned int j = 0; j < mEstimate.getDimensions()[1]; ++j)
  {
    for(unsigned int i = 0; i < mEstimate.getDimensions()[0]; ++i)
    {
      float &p = mEstimate.raster(i,j);

      // note that this is not biased, it's defensive sampling
      p = 1.0f / std::max(estimate->raster(i,j).luminance(), 0.01f);
    } // end for i
  } // end for i

  // now call the Parent
  Parent::preprocess(r, scene, context, mutator, renderer);
} // end NormalizedImportance::preprocess()

float NormalizedImportance
  ::evaluate(const PathSampler::HyperPoint &x,
             const Path &xPath,
             const std::vector<PathSampler::Result> &results)
{
  PathToImage mapToImage;
  Spectrum I(0,0,0);
  gpcpu::float2 bucket;
  for(std::vector<PathSampler::Result>::const_iterator r = results.begin();
      r != results.end();
      ++r)
  {
    // scale each result by its corresponding bucket
    mapToImage(*r, x, xPath, bucket[0], bucket[1]);
    I += (r->mThroughput * r->mWeight / r->mPdf) * mEstimate.element(bucket[0], bucket[1]);
  } // end for r

  return I.luminance();
} // end NormalizedImportance::operator()()

