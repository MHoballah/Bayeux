// -*- mode: c++; -*-
/* the_serializable.cc
 */

#include <cstdlib>
#include <iostream>

#include <boost/scoped_ptr.hpp>

// Load the Boost/Serialization instantiation and registration code :
#include <geomtools/the_serializable.h>

// Load the link guard definition :
#include <geomtools/detail/bio_link_guard.h>

// Load the link guard implementation :
namespace geomtools {
  namespace detail {
    namespace serialization {

      dynamic_link_guard::dynamic_link_guard ()
      {
        return;
      }

      dynamic_link_guard::~dynamic_link_guard ()
      {
        return;
      }

      dynamic_link_guard & dynamic_link_guard::instance ()
      {
        static boost::scoped_ptr<dynamic_link_guard> g_global_guard (0);
        if ( g_global_guard.get () == 0)
          {
            g_global_guard.reset (new dynamic_link_guard);
          }
        return *g_global_guard.get ();
      }

    } // end namespace serialization
  } // end namespace detail
} // end namespace geomtools

// end of the_serializable.cc
