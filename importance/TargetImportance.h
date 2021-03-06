/*! \file TargetImportance.h
 *  \author Jared Hoberock
 *  \brief Defines the interface to a ScalarImportance class
 *         which assigns importance based on a target sampling rate.
 */

#ifndef TARGET_IMPORTANCE_H
#define TARGET_IMPORTANCE_H

#include "EstimateImportance.h"
#include "../records/RenderFilm.h"

class TargetImportance
  : public ScalarImportance
{
  public:
    /*! \typedef Parent
     *  \brief Shorthand.
     */
    typedef ScalarImportance Parent;

    /*! Constructor takes a reference to a RandomAccessFilm containing
     *  an estimate of the image distribution and a RandomAccessFilm containing
     *  a target sampling distribution.
     *  \param estimate Sets mEstimate.
     *  \param target Sets mTarget.
     *  \param filename The file to write to on postprocess.
     */
    TargetImportance(const RandomAccessFilm &estimate,
                     const RandomAccessFilm &target,
                     const std::string &filename = "");

    /*! This method converts the spectral Monte Carlo throughput
     *  of a Path into scalar importance.
     *  \param x The HyperPoint uniquely specifying the Path of interest.
     *  \param f The spectral Monte Carlo throughput of the Path of interest.
     *  \return The scalar importance of x.
     */
    virtual float evaluate(const PathSampler::HyperPoint &x,
                           const Path &xPath,
                           const std::vector<PathSampler::Result> &results);

    /*! This method calls mEstimateImportance.preprocess(), then calls
     *  the Parent.
     *  \param r A sequence of RandomNumbers.
     *  \param scene The Scene to be rendered.
     *  \param context A ShadingContext for evaluating shaders.
     *  \param mutator The PathMutator to be used duing the rendering process.
     *  \param renderer A reference to the MetropolisRenderer owning this
     *                  ScalarImportance.
     */
    virtual void preprocess(const boost::shared_ptr<RandomSequence> &r,
                            const boost::shared_ptr<const Scene> &scene,
                            const boost::shared_ptr<ShadingContext> &context,
                            const boost::shared_ptr<PathMutator> &mutator,
                            MetropolisRenderer &renderer);

    /*! This method is called after rendering and writes mTarget to
     *  mTargetFilename.
     */
    virtual void postprocess(void);

  protected:
    /*! This maps a Path to an image location.
     */
    PathToImage mMapToImage;

    /*! This is one half of the importance.
     */
    EstimateImportance mEstimateImportance;

    RandomAccessFilm mTarget;

    /*! The filename to write on postprocess().
     */
    std::string mTargetFilename;
}; // end TargetImportance

#endif // TARGET_IMPORTANCE_H

