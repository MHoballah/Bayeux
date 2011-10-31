// -*- mode: c++; -*- 
/* mygsl::interval.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2009-10-02
 * 
 * License: 
 * 
 * 
 * Description: 
 *
 *   A simple interval [min:max] for double precision values
 *
 * [0:1]
 * (0:1]
 * [0:1)
 * ( :1]
 * [0: )
 * [0: )
 * ( : )
 * [!]
 *
 * History: 
 * 
 *
 *
 */

#ifndef __mygsl__interval_h
#define __mygsl__interval_h 1

#include <iostream>

using namespace std;

namespace mygsl {

  class interval
    {
    public:
      static const double NO_MIN_VALUE;
      static const double NO_MAX_VALUE;
      static const double NO_VALUE;
      static const double DEFAULT_EPS;
      static const double AUTO_EPS;
      static const double auto_eps;

      static const char IO_SEP;
      static const char IO_EMPTY;
      static const char IO_OPEN_INCL;
      static const char IO_OPEN_EXCL;
      static const char IO_CLOSE_INCL;
      static const char IO_CLOSE_EXCL;

      static const bool included;
      static const bool excluded;

    private:

      bool   _min_included_;
      bool   _max_included_;
      double _min_;
      double _max_;
      double _eps_;

    public:

      // ctor:
      interval ();

      // ctor:
      interval (double min_, double max_, double eps_ = AUTO_EPS);

      // ctor:
      interval (double min_, bool min_include_, 
		double max_, bool max_include_, 
		double eps_ = AUTO_EPS);

      // dtor:
      virtual ~interval ();

      void remove_min ();

      void remove_max ();

      void reset ();

      void set_min_included (bool inc_);

      void set_max_included (bool inc_);

      bool is_min_included () const;

      bool is_max_included () const;

      bool is_valid () const;

      bool has_min () const;

      bool has_max () const;

      double get_min () const;

      double get_max () const;

      double get_eps () const;
      
      bool union_with (const interval &);
      
      bool intercept_with (const interval &);

      bool is_in (double x_) const;

      bool is_in_safe (double x_) const;

      bool is_in_min_unsafe (double x_) const;

      bool is_in_max_unsafe (double x_) const;

      interval get_safe_interval (double eps_ = AUTO_EPS) const;

      void set_eps (double eps_);

      void set (double min_, double max_, double eps_ = AUTO_EPS);

      bool is_empty () const;

      void open ();

      void close ();

      bool intersection_with (const interval & i_) const;

      static interval make_empty ();

      static interval make_no_limit (double eps_ = AUTO_EPS);

      static interval make_neighbourhood (double value_, double delta_, bool included_ = excluded);

      static interval make_min (double min_, bool min_include_ = excluded, 
				double eps_ = AUTO_EPS);

      static interval make_max (double max_, bool max_include_ = excluded, 
				double eps_ = AUTO_EPS);

      static interval make_min_max_included (double min_,
					     double max_,
					     double eps_ = AUTO_EPS);

      static interval make_min_max_excluded (double min_,
					     double max_,
					     double eps_ = AUTO_EPS);

      static interval make_min_max (double min_, bool min_include_,
				    double max_, bool max_include_,
				    double eps_ = AUTO_EPS);

      friend ostream & operator<< (ostream & out_, const interval & i_);

      friend istream & operator>> (istream & in_, interval & i_);

    };

} // end of namespace mygsl

#endif // __mygsl__interval_h

// end of mygsl::interval.h
