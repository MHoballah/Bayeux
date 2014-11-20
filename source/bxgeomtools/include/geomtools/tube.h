// -*- mode: c++; -*-
/// \file geomtools/tube.h
/* Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2014-11-19
 *
 * License:
 *
 * Description:
 *   3D tube description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_TUBE_H
#define GEOMTOOLS_TUBE_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declaration:
  class cylinder;

  /// \brief The 3D shape model for a tube
  class tube : public i_shape_3d, public i_stackable
  {
  public:
    enum faces_mask_type
      {
        FACE_NONE       = geomtools::FACE_NONE,
        FACE_OUTER_SIDE = datatools::bit_mask::bit00,
        FACE_BOTTOM     = datatools::bit_mask::bit01,
        FACE_TOP        = datatools::bit_mask::bit02,
        FACE_INNER_SIDE = datatools::bit_mask::bit03,
        FACE_ALL        = (FACE_OUTER_SIDE
                           | FACE_BOTTOM
                           | FACE_TOP
                           | FACE_INNER_SIDE)
      };

  public:

    static const std::string & tube_label();

  public:

    double get_xmin () const;

    double get_xmax () const;

    double get_ymin () const;

    double get_ymax () const;

    double get_zmin () const;

    double get_zmax () const;

    double get_z () const;

    void set_z (double);

    double get_half_z() const;

    void set_half_z (double);

    double get_inner_r () const;

    void set_radii (double inner_r_, double outer_r_);

    double get_outer_r () const;

    void set (double inner_r_, double outer_r_, double z_);

    void set_half (double inner_r_, double outer_r_, double half_z_);

    void set (double inner_r_, double outer_r_, double z_, double start_phi_, double delta_phi_);

    void set_phi(double start_phi_, double delta_phi_);

    double get_start_phi() const;

    double get_delta_phi() const;

    bool has_partial_phi() const;

    void compute_inner_cylinder (cylinder & ic_);

    void compute_outer_cylinder (cylinder & oc_);

  public:

    /// Default constructor
    tube();

    /// Constructor
    tube (double inner_radius_, double outer_radius_, double z_);

     /// Constructor
    tube (double inner_radius_, double outer_radius_, double z_, double start_phi_, double delta_phi_);

   /// Destructor
    virtual ~tube ();

    virtual std::string get_shape_name () const;

    virtual double get_parameter (const std::string &) const;

    bool is_extruded () const;

    bool is_valid () const;

    /// Initialize the cylinder from properties
    virtual void initialize(const datatools::properties &, const handle_dict_type * = 0);

    /// Reset the cylinder
    virtual void reset();

    virtual double get_surface (uint32_t mask_ = FACE_ALL_BITS) const;

    virtual double get_volume (uint32_t flags_ = 0) const;

    virtual bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside (const vector_3d &,
                             double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface (const vector_3d &,
                                int mask_    = FACE_ALL,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    friend std::ostream &
    operator<< (std::ostream &, const tube &);

    friend std::istream &
    operator>> (std::istream &, tube &);

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    virtual void generate_wires (std::list<polyline_3d> &,
                                 const placement & ,
                                 uint32_t options_ = 0) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

     virtual void _build_bounding_data();

  private:

    double _z_;         //!< Length of the tube along the Z axis
    double _inner_r_;   //!< Inner radius
    double _outer_r_;   //!< Outer radius
    double _start_phi_; //<! Start phi angle
    double _delta_phi_; //<! Delta phi angle
    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(tube);

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::tube the name the class
DOCD_CLASS_DECLARATION(geomtools::tube)

#endif // GEOMTOOLS_TUBE_H
