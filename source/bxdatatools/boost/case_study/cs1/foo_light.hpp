// foo_light.hpp
// -*- mode: c++; -*-
// (C) Copyright 2009 François Mauger <mauger@lpccaen.in2p3.fr>
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/* Some classes to illustrate the memory tracking mechanism
 * while serializing data with the Boost serialization library.
 *
 * Class 'fuu' has:
 * - a single double in range [0:1)
 * Class 'foo' has:
 * - a STL list of dynamically allocated 'fuu' instances 
 * - some fields that are automatically computed from the 
 *   current list of 'fuus' (sum of values and pointer to the instance
 *   with max value)
 *
 */

#ifndef __foo_light_h__
#define __foo_light_h__ 1

#include <cstdlib>
#include <string>
#include <list>

#include <boost/cstdint.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>

using namespace std;

class fuu
{
private:

  int32_t __value;

public:

  fuu ()
  {
    __value = 0;
  }

  ~fuu ()
  {
  }

  fuu (int32_t v_)
  {
    set_value (v_);
  }

  void set_value (int32_t v_)
  {
    __value = v_;
  }

  int32_t get_value () const
  {
    return __value;
  }

private:
  friend class boost::serialization::access; 

  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("value", __value);
  }
};

ostream & operator<< (ostream & out_, const fuu & f_)
{
  out_ << "(value=" << f_.get_value () << ")";
  return out_;
}

class foo
{
private:
  list<fuu*> __fuus;
  int32_t    __sum;
  fuu*       __max_fuu;

public:

  foo ()
  {
    __sum = 0;
    __max_fuu = 0;
  }

  void clear_fuus ()
  {
    for (list<fuu*>::iterator it = __fuus.begin ();
	 it != __fuus.end ();
	 it++)
      {
	if (*it != 0)
	  {
	    delete *it;
	    *it = 0;
	  }
      }
    __fuus.clear ();
    __sum = 0;
    __max_fuu = 0;
  }

  ~foo ()
  {
    if (__fuus.size () > 0) clear_fuus ();
  }

  int32_t check_sum () const
  {
    int32_t s = 0;    
    for (list<fuu*>::const_iterator it = __fuus.begin ();
	 it != __fuus.end ();
	 it++)
      {
	const fuu * f = *it;
	s += f->get_value ();
      }
    return s;
  }

  void init_fuus (int n_)
  {
    __sum = 0;
    __max_fuu = 0;
    int32_t max_val = 0;
    for (int i = 0; i < n_; i++)
      {
	int32_t val = (int32_t) (drand48 () * 100);
	fuu * f = new fuu (val);
	if ((val > max_val) || (__max_fuu == 0))
	  {
	    max_val = val;
	    __max_fuu = f;
	  }
	__fuus.push_back (f);
	__sum += val; 
      }
  }

  const list<fuu*> & get_fuus () const
  {
    return __fuus;
  }

  list<fuu*> & get_fuus ()
  {
    return __fuus;
  }

  const fuu * get_max_fuu () const
  {
    return __max_fuu;
  }

  int32_t get_sum () const
  {
    return __sum;
  }

  friend ostream & operator<< (ostream &, const foo &);

private:
  friend class boost::serialization::access; 

  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("fuus", __fuus);
    ar_ & boost::serialization::make_nvp ("sum", __sum);
    ar_ & boost::serialization::make_nvp ("max_fuu", __max_fuu);
  }
};

std::map<fuu*,int> foo::g_addresses;

ostream & operator<< (ostream & out_, const foo & f_)
{
  size_t check_clear = false; // flag to detect '__max_fuu' pointer missing ownership
  out_ << "(sum=" << f_.get_sum ();
  int count = 0;
  for (list<fuu*>::const_iterator it = f_.get_fuus ().begin ();
       it != f_.get_fuus ().end ();
       it++)
    {
      if (*it == f_.__max_fuu)
	{
	  check_clear = true;
	}
      out_ << ", fuu[" << *it << "]=";
      out_ << **it;
      count++;
    }
  int32_t sum = f_.get_sum ();
  int32_t check_sum = f_.check_sum ();
  out_ << ", max_fuu=[" << f_.get_max_fuu () << ']';
  out_ << ", check_sum=" << check_sum;
  out_ << ", status=" << (((sum == check_sum) && check_clear) ? "OK" : "ERROR");
  out_ << ')';
  return out_;
}

#endif // __foo_light_h__

// end of foo_light.hpp
