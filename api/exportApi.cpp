/*! \file exportApi.h
 *  \author Jared Hoberock
 *  \brief Implementation of exportApi function.
 */

#include "exportApi.h"
#include "Gotham.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

// wrapper for Gotham::material()
void Gotham_material(Gotham &g, std::auto_ptr<Material> m)
{
  g.material(m.get());
  m.release();
} // end Gotham_material()

// tell boost which multMatrix we mean
typedef void (Gotham::*multMatrix_vector)(const std::vector<float>&);

// tell boost which loadMatrix we mean
typedef void (Gotham::*loadMatrix_vector)(const std::vector<float>&);

// tell boost which getMatrix we mean
typedef void (Gotham::*getMatrix_vector)(const std::vector<float>&);

// deal with overloaded Gotham::mesh
void (Gotham::*mesh2)(std::vector<float>&,
                      std::vector<unsigned int>&)
  = &Gotham::mesh;

void (Gotham::*mesh3)(std::vector<float>&,
                      std::vector<float>&,
                      std::vector<unsigned int> &)
  = &Gotham::mesh;

void exportGotham(void)
{
  class_<Gotham>("Gotham")
    .def("pushMatrix", &Gotham::pushMatrix)
    .def("popMatrix", &Gotham::popMatrix)
    .def("translate", &Gotham::translate)
    .def("rotate", &Gotham::rotate)
    .def("scale", &Gotham::scale)
    .def("multMatrix", multMatrix_vector(&Gotham::multMatrix))
    .def("loadMatrix", loadMatrix_vector(&Gotham::loadMatrix))
    .def("getMatrix", getMatrix_vector(&Gotham::getMatrix))
    .def("mesh", mesh2)
    .def("mesh", mesh3)
    .def("sphere", &Gotham::sphere)
    .def("render", &Gotham::render)
    .def("material", Gotham_material)
    .def("attribute", &Gotham::attribute)
    .def("pushAttributes", &Gotham::pushAttributes)
    .def("popAttributes", &Gotham::popAttributes)
    ;
} // end exportGotham()

void exportMaterial(void)
{
  class_<Material, std::auto_ptr<Material> >("Material")
    ;
} // end exportMaterial()

void exportVectorFloat(void)
{
  class_<std::vector<float> >("vector_float")
    .def(vector_indexing_suite<std::vector<float> >())
    ;
} // end exportVectorFloat()

void exportVectorUint(void)
{
  class_<std::vector<unsigned int> >("vector_uint")
    .def(vector_indexing_suite<std::vector<unsigned int> >())
    ;
} // end exportVectorUint()

void exportApi(void)
{
  exportGotham();
  exportMaterial();
  exportVectorFloat();
  exportVectorUint();
} // end exportApi()

BOOST_PYTHON_MODULE(gotham)
{
  exportApi();
} // end BOOST_PYTHON_MODULE()
