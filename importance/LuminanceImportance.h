/*! \file LuminanceImportance.h
 *  \author Jared Hoberock
 *  \brief Defines the interface to a ScalarImportance
 *         class which assigns importance equal
 *         to luminance.
 */

#ifndef LUMINANCE_IMPORTANCE_H
#define LUMINANCE_IMPORTANCE_H

#include "ScalarImportance.h"

class LuminanceImportance
  : public ScalarImportance
{
  public:
    /*! \typedef Parent
     *  \brief Shorthand.
     */
    typedef ScalarImportance Parent;

    /*! This method converts the spectral Monte Carlo throughput
     *  of a Path into scalar importance by returning its luminance.
     *  \param x The HyperPoint uniquely specifying the Path of interest.
     *  \param f The spectral Monte Carlo throughput of the Path of interest.
     *  \return The luminance importance of x.
     */
    virtual float operator()(const PathSampler::HyperPoint &x,
                             const Spectrum &f);
}; // end LuminanceImportance

#endif // LUMINANCE_IMPORTANCE_H
