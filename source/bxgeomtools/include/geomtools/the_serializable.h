// -*- mode: c++; -*- 
/* geomtool::the_serializable.h */

#ifndef __geomtools__the_serializable_h__
#define __geomtools__the_serializable_h__ 1

#include <boost/serialization/export.hpp>

#include <datatools/serialization/archives_instantiation.h>
#include <datatools/base_serializable.h>


/**********************
 * geomtools::geom_id *
 **********************/

#include <geomtools/geom_id.ipp>

BOOST_CLASS_EXPORT_KEY2(geomtools::geom_id, "geomtools::geom_id")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::geom_id)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::geom_id) 

#include <geomtools/utils.ipp>

BOOST_CLASS_EXPORT_KEY2(geomtools::vector_2d, "geomtools::vector_2d")
DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_2d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::vector_2d) 

BOOST_CLASS_EXPORT_KEY2(geomtools::vector_3d, "geomtools::vector_3d")
DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::vector_3d) 

BOOST_CLASS_EXPORT_KEY2(geomtools::rotation_3d, "geomtools::rotation_3d")
DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::rotation_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::rotation_3d) 


#include <geomtools/helix_3d.ipp>

#include <geomtools/line_3d.ipp>

#include <geomtools/polyline_3d.ipp>


#endif // __geomtools__the_serializable_h__

/* end of geomtool::the_serializable.h */
