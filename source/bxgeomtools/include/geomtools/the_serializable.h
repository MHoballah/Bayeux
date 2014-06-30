// -*- mode: c++; -*-
/// \file geomtools/the_serializable.h

#ifndef GEOMTOOLS_THE_SERIALIZABLE_H
#define GEOMTOOLS_THE_SERIALIZABLE_H 1

// Third party:
// - Boost:
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
#include <geomtools/the_serializable.ipp>

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_2d)

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_3d)

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::rotation_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::geom_id)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::geom_id)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::line_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::line_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::helix_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::helix_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::polyline_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::polyline_3d)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::i_placement)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::i_placement)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::placement)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::placement)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::blur_spot)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::blur_spot)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::base_hit)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::base_hit)

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::display_data)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::display_data)

#endif // GEOMTOOLS_THE_SERIALIZABLE_H
