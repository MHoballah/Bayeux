// -*- mode: c++; -*- 
/* box.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface:
 *
 * History: 
 * 
 */

#ifndef __geomtools__box_h
#define __geomtools__box_h 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  class box : public i_shape_3d ,
	      public i_stackable
  {

  public:
    static const std::string BOX_LABEL;
    enum faces_mask_t
      {
	FACE_NONE   = FACE_NONE_BIT,
	FACE_BACK   = 0x1,
	FACE_FRONT  = 0x2,
	FACE_LEFT   = 0x4,
	FACE_RIGHT  = 0x8,
	FACE_BOTTOM = 0x10,
	FACE_TOP    = 0x20,
	FACE_ALL    = (FACE_BACK 
		       | FACE_FRONT 
		       | FACE_LEFT 
		       | FACE_RIGHT 
		       | FACE_BOTTOM 
		       | FACE_TOP)
      };

  private: 

    double __x;
    double __y;
    double __z;

  public: 
    
    double get_xmin () const
    {
      return -0.5*__x;
    }
    
    double get_xmax () const
    {
      return +0.5*__x;
    }
    
    double get_ymin () const
    {
      return -0.5*__y;
    }
    
    double get_ymax () const
    {
      return +0.5*__y;
    }
    
    double get_zmin () const
    {
      return -0.5*__z;
    }
    
    double get_zmax () const
    {
      return +0.5*__z;
    }

    double get_x () const;

    void set_x (double);

    double get_half_x () const;

    void set_half_x (double);

    double get_y () const;

    void set_y (double);

    double get_half_y () const;

    void set_half_y (double);

    double get_z () const;

    void set_z (double);

    double get_half_z () const;

    void set_half_z (double);

    void set (double, double, double);

    void set_half (double, double, double);
  
  public: 

    // ctor:
    box ();

    // ctor:
    box (double x_, double y_, double z_);

    // dtor:
    virtual ~box ();
    
    // methods:
      
    virtual std::string get_shape_name () const;

    double get_parameter (const std::string &) const;

    bool is_valid () const;

    void init ();

    void reset ();

    double get_surface (int mask_ = FACE_ALL) const;

    double get_volume () const;

    virtual bool is_inside (const vector_3d &, 
	       double skin_ = USING_PROPER_SKIN) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface (const vector_3d & , 
		   int mask_    = FACE_ALL , 
		   double skin_ = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    friend std::ostream & operator<< ( std::ostream & , const box & );

    friend std::istream & operator>> ( std::istream & , box & );
      
    virtual bool find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double skin_ = USING_PROPER_SKIN) const;

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
       
  };
    
} // end of namespace geomtools

#endif // __geomtools__box_h

// end of box.h

