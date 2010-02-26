// -*- mode: c++; -*- 
/* placement.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__placement_h
#define __geomtools__placement_h 1

#include <iostream>
#include <iomanip>
#include <string>

#include <geomtools/i_placement.h>

namespace geomtools {

  using namespace std;

  class placement
    : public i_placement
    {
    private: 
      vector_3d    __translation;      // absolute position in mother frame
      double       __phi, __theta, __delta;
      rotation_3d  __rotation;         // mother->child frame coord. transformation
      rotation_3d  __inverse_rotation; // child->mother frame coord. transformation
    public:
 
      bool is_valid () const;

      void invalidate ();

      const vector_3d & get_translation () const;

      void set_translation (double, double, double);

      void set_translation (const vector_3d &);

      double get_phi () const;

      double get_theta () const;

      double get_delta () const;

      bool has_angles () const;

      const rotation_3d & get_rotation () const;

      const rotation_3d & get_inverse_rotation () const;

    public: 
      // i_placement interface:
      virtual size_t get_number_of_items () const;
   
      virtual void get_placement (int item_, placement & p_) const;
 
    private:
      void __compute_orientation ();

    public:

      void set_orientation (double phi_, double theta_, double delta_);

      void set (double x_, double y_, double z_, 
		double phi_, double theta_, double delta_);

      void set (const vector_3d & t_, 
		double phi_, double theta_, double delta_);

      // Not recommended at all:
      void set_orientation (const rotation_3d &);

    public: 
      // ctor:
      placement ();

      // ctor:
      placement (const vector_3d & translation_, 
		 double phi_,
		 double theta_,
		 double delta_);

      // ctor:
      placement (double x_,
		 double y_,
		 double z_, 
		 double phi_,
		 double theta_,
		 double delta_);

      // dtor:
      virtual ~placement ();

      virtual void reset ();
  
      //  transformation methods:
      void mother_to_child (const vector_3d &, vector_3d &) const;

      vector_3d mother_to_child (const vector_3d &) const;

      void child_to_mother (const vector_3d &, vector_3d &) const;

      vector_3d child_to_mother (const vector_3d &) const;

      void mother_to_child_direction (const vector_3d &, vector_3d &) const;

      vector_3d mother_to_child_direction (const vector_3d &) const;

      void child_to_mother_direction (const vector_3d &, vector_3d &) const;

      vector_3d child_to_mother_direction (const vector_3d &) const;

      //void mother_to_child (const placement &, placement &) const;

      void child_to_mother (const placement &, placement &) const;

      // i_tree_dump interface:
      virtual void tree_dump (ostream & out_  = clog, 
			      const string & title_ = "geomutils::placement", 
			      const string & indent_ = "", 
			      bool inherit_ = false) const;

      void dump (ostream      & out_    = clog, 
		 const string & title_  = "geomutils::placement", 
		 const string & indent_ = "") const;
    
    };

} // end of namespace geomtools

#endif // __geomtools__placement_h

// end of placement.h
