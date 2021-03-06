/*! \file SpecularReflection.h
 *  \author Jared Hoberock
 *  \brief Defines the interface to a scattering function
 *         implementing specular reflection.
 */

#ifndef SPECULAR_REFLECTION_H
#define SPECULAR_REFLECTION_H

#include "DeltaDistributionFunction.h"

class Fresnel;

class SpecularReflection
  : public DeltaDistributionFunction
{
  public:
    /*! \typedef Parent
     *  \brief Shorthand.
     */
    typedef DeltaDistributionFunction Parent;

    /*! This constructor creates a SpecularReflection conductor.
     *  \param r Sets mReflectance.
     *  \param eta Sets the index of refraction of the Fresnel conductor.
     *  \param alloc A FunctionAllocator for creating the Fresnel object.
     */
    SpecularReflection(const Spectrum &r, const float eta, FunctionAllocator &alloc);

    /*! This constructor creates a SpecularReflection dielectric.
     *  \param r Sets mReflectance.
     *  \param etai Sets the index of refraction of the space surrounding the dielectric.
     *  \param etat Sets the index of refraction of the Fresnel dielectric medium.
     *  \param alloc A FunctionAllocator for creating the Fresnel object.
     */
    SpecularReflection(const Spectrum &r, const float etai, const float etat, FunctionAllocator &alloc);

    /*! This method evaluates a SpecularReflection reflectange function.
     *  \param wi A vector pointing towards the direction of incoming radiance.
     *  \param dg The DifferentialGeometry at the surface point of interest.
     *  \param wo A vector pointing towards the viewing direction.
     *  \return The scattering in direction wo.
     */
    using Parent::evaluate;
    virtual Spectrum evaluate(const Vector &wo,
                              const DifferentialGeometry &dg,
                              const Vector &wi) const;

    /*! This method samples this SpecularReflection distribution function given a wo,
     *  DifferentialGeometry, and three numbers in the unit interval.
     *  \param wo The direction of scattering.
     *  \param dg The DifferentialGeometry at the point of interest.
     *  \param u0 A real number in [0,1).
     *  \param u1 A second real number in [0,1).
     *  \param u2 A third real number in [0,1).
     *  \param wi The direction of scattering is returned here.
     *  \param pdf The value of the pdf at (u0,u1,u2) is returned here.
     *  \param delta This is set to true.
     *  \param component This is set to 0.
     *  \return The bidirectional scattering from wi to wo is returned here.
     */
    using Parent::sample;
    virtual Spectrum sample(const Vector &wo,
                            const DifferentialGeometry &dg,
                            const float u0,
                            const float u1,
                            const float u2,
                            Vector &wi,
                            float &pdf,
                            bool &delta,
                            ComponentIndex &index) const;

    /*! This method computes the value of this SpecularReflection and its pdf.
     *  \param wo A Vector pointing towards the direction of scattering.
     *  \param dg The DifferentialGeometry at the scattering Point of interest.
     *  \param wi A Vector pointing towards the direction of incidence.
     *  \param delta When this is true, 
     *  \param component Ignored.  SpecularReflection has only one component.
     *  \param pdf The value of this SpecularReflection's pdf is returned here.
     *  \return The value of this SpecularReflection along (wo,dg,wi).
     */
    virtual Spectrum evaluate(const Vector &wo,
                              const DifferentialGeometry &dg,
                              const Vector &wi,
                              const bool delta,
                              const ComponentIndex component,
                              float &pdf) const;

  protected:
    /*! The reflectance.
     */
    Spectrum mReflectance;

    /*! A pointer to the Fresnel object.
     *  \note Since this is allocated by a pool,
     *        we don't need to delete it.
     *  XXX BUG this needs to be cloned correctly
     */
    Fresnel *mFresnel;
}; // end SpecularReflection

#endif // SPECULAR_REFLECTION_H

