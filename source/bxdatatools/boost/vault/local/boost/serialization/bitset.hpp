/*!
 * \file      bitset.hpp
 * \brief     Provides Boost.Serialization support for std::bitset
 * \author    Brian Ravnsgaard Riis
 *            Modified by François Mauger for XML archive support (nvp)
 * \date      16.09.2004
 * \copyright 2004 Brian Ravnsgaard Riis
 * \license   Boost Software License 1.0
 */
#ifndef BOOST_SERIALIZATION_BITSET_HPP
#define BOOST_SERIALIZATION_BITSET_HPP

#include <boost/config.hpp>
#include <boost/serialization/split_free.hpp>

// 2009-11-13 FM: add
#include <boost/serialization/nvp.hpp>

#include <bitset>

#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#define STD _STLP_STD
#else
#define STD std
#endif

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
namespace boost {
  namespace serialization {
#else
namespace STD {
#endif

    template <class Archive, unsigned size>
    inline void save(Archive & ar,
        STD::bitset<size> const & t,
        const unsigned int version)
    {
      for(unsigned i = 0; i < size; ++i)
	{
	  // 2009-11-13 FM: add const reference + 'make_nvp'
	  //>>>
	  const bool & bit_ref = t.test(i);
	  ar & boost::serialization::make_nvp ("bit", bit_ref);
	  //<<<
	}
    }

    template <class Archive, unsigned size>
    inline void load(Archive & ar,
        STD::bitset<size> & t,
        const unsigned int version)
    {
      bool temp;
      for(unsigned i = 0; i < size; ++i)
      {
	// 2009-11-13 FM: add 'make_nvp'
	//>>>
	ar & boost::serialization::make_nvp ("bit", temp);
	//<<<
        t.set(i, temp);
      }
    }

    template <class Archive, unsigned size>
    inline void serialize(Archive & ar,
        STD::bitset<size> & t,
        const unsigned int version)
    {
      boost::serialization::split_free(ar, t, version);
    }

#ifndef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
} // namespace STD 
#else
  } // namespace serialization
} // namespace boost
#endif

#endif // BOOST_SERIALIZATION_BITSET_HPP

