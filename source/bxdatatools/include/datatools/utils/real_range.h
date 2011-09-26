// -*- mode: c++; -*- 
/* datatools/utils/real_range.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-09-25
 * Last modified: 2011-09-25
 * 
 * License: 
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *
 *   Real range/interval class.
 * 
 * History: 
 *  
 */

#include <iostream>
#include <string>
#include <set>

#include <boost/cstdint.hpp>

#include <datatools/utils/range_tools.h>

using namespace std;

namespace datatools {

  namespace utils {

    class real_range
    {
    public:

      static const double DEFAULT_EPSILON;

      typedef double value_type;

      bool is_valid () const;
 
      void invalidate ();

      void reset_lower ();
 
      void reset_upper ();
 
      void reset ();
 
      void set_lower (value_type a_lower_value, 
		      int a_lower_flag = range::included);

      void set_upper (value_type a_upper_value, 
		      int a_upper_flag = range::included);

      void set (value_type a_lower_value, 
		value_type a_upper_value, 
		int a_lower_flag = range::included, 
		int a_upper_flag = range::included);

      // Ctor:
      real_range ();

      // Ctor:
      real_range (value_type a_lower_value, 
		  value_type a_upper_value, 
		  int a_lower_flag = range::included, 
		  int a_upper_flag = range::included);

      bool is_lower_bounded () const;

      bool is_upper_bounded () const;

      bool is_bounded () const;

      bool is_lower_included () const;

      bool is_upper_included () const;

      value_type get_lower_bound () const;

      value_type get_upper_bound () const;

      //bool is_empty () const;

      // Collections of 'make-methods' :

      // "];["
      void make_empty ();

      // "[min_int;max_int]"
      void make_full ();

      // "[0;max_int]"
      void make_full_positive ();

      // "[min_int;0]"
      void make_full_negative ();

      // "[lower;[" or "]lower;[
      void make_upper_unbounded (value_type a_lower_value, 
				 bool a_lower_included = true);

      // "];upper]" or "];upper["
      void make_lower_unbounded (value_type a_upper_value, 
				 bool a_upper_included = true);

      // "]lower;upper]" or "]lower;upper["
      // "[lower;upper]" or "[lower;upper["
      void make_bounded (value_type a_lower_value, 
			 value_type a_upper_value,
			 bool a_lower_included = true, 
			 bool a_upper_included = true);

      bool has (value_type a_value) const;

      void dump (ostream & a_out = clog) const;
			
			static double compute_epsilon (double a_lower,
																		 double a_upper);

      friend ostream & operator<< (ostream & a_out, 
				   const real_range & a_range); 

    private:

      value_type _abs_epsilon_;
      char       _lower_flag_;
      value_type _lower_;
      char       _upper_flag_;
      value_type _upper_;
 
    };

 
  } // end of namespace utils 

} // end of namespace datatools 

// end of datatools/utils/real_range.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
