/// geomtools/ellipsoid.cc

// Ourselves:
#include <geomtools/ellipsoid.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/placement.h>
#include <geomtools/i_shape_2d.h>
#include <geomtools/elliptical_sector.h>
#include <geomtools/ellipsoid_sector.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(ellipsoid, "geomtools::ellipsoid")

  const std::string & ellipsoid::ellipsoid_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "ellipsoid";
    }
    return label;
  }

  void ellipsoid::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    return;
  }

  double
  ellipsoid::get_xmin() const
  {
    return -get_x_radius();
  }

  double
  ellipsoid::get_xmax() const
  {
    return get_x_radius();
  }

  double
  ellipsoid::get_ymin() const
  {
    return -get_y_radius();
  }

  double
  ellipsoid::get_ymax() const
  {
    return get_y_radius();
  }

  double
  ellipsoid::get_zmin() const
  {
    if (datatools::is_valid(_bottom_z_cut_)) {
      return _bottom_z_cut_;
    }
    return -get_z_radius();
  }

  double
  ellipsoid::get_zmax() const
  {
    if (datatools::is_valid(_top_z_cut_)) {
      return _top_z_cut_;
    }
    return get_z_radius();
  }

  double
  ellipsoid::get_x_radius() const
  {
    return _x_radius_;
  }

  void
  ellipsoid::set_x_radius (double r_)
  {
    DT_THROW_IF(r_ <= 0.0, std::domain_error,
                "Invalid radius (" << r_ << ") !");
    _x_radius_ = r_;
    return;
  }

  double
  ellipsoid::get_y_radius() const
  {
    return _y_radius_;
  }

  void
  ellipsoid::set_y_radius (double r_)
  {
    DT_THROW_IF(r_ <= 0.0, std::domain_error,
                "Invalid radius (" << r_ << ") !");
    _y_radius_ = r_;
    return;
  }

  double
  ellipsoid::get_z_radius() const
  {
    return _z_radius_;
  }

  void
  ellipsoid::set_z_radius (double r_)
  {
    DT_THROW_IF(r_ <= 0.0, std::domain_error,
                "Invalid radius (" << r_ << ") !");
    _z_radius_ = r_;
    return;
  }

  double ellipsoid::get_bottom_z_cut() const
  {
    return _bottom_z_cut_;
  }

  void ellipsoid::set_bottom_z_cut(double cut_)
  {
    DT_THROW_IF(cut_ <= -_z_radius_, std::logic_error, "Invalid bottom Z cut!");
    if (datatools::is_valid(_top_z_cut_)) {
      DT_THROW_IF(cut_ >= _top_z_cut_, std::logic_error, "Invalid bottom Z cut (> top Z cut)!");
    }
    _bottom_z_cut_ = cut_;
    return;
  }

  double ellipsoid::get_top_z_cut() const
  {
    return _top_z_cut_;
  }

  void ellipsoid::set_top_z_cut(double cut_)
  {
    DT_THROW_IF(cut_ >= + _z_radius_, std::logic_error, "Invalid top Z cut!");
    if (datatools::is_valid(_bottom_z_cut_)) {
      DT_THROW_IF(cut_ <= _bottom_z_cut_, std::logic_error, "Invalid top Z cut (> bottom Z cut)!");
    }
    _top_z_cut_ = cut_;
    return;
  }

  void
  ellipsoid::reset()
  {
    unlock();

    _set_default();

    this->i_shape_3d::_reset();
    return;
  }

  void ellipsoid::_set_default()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    datatools::invalidate(_z_radius_);
    datatools::invalidate(_bottom_z_cut_);
    datatools::invalidate(_top_z_cut_);
    return;
  }

  void ellipsoid::set(double rx_, double ry_, double rz_)
  {
    _set_default();
    set_x_radius(rx_);
    set_y_radius(ry_);
    set_z_radius(rz_);
    return;
  }

  void ellipsoid::set (double rx_, double ry_, double rz_,
                       double zm_, double zp_)
  {
    set(rx_, ry_, rz_);
    set_bottom_z_cut(zm_);
    set_top_z_cut(zp_);
    return;
  }

  ellipsoid::ellipsoid()
  {
    _set_default();
    return;
  }

  ellipsoid::ellipsoid (double rx_, double ry_, double rz_)
  {
    _set_default();
    set(rx_, ry_, rz_);
    return;
  }

  ellipsoid::ellipsoid(double rx_, double ry_, double rz_,
                       double zm_, double zp_)
  {
    _set_default();
    set(rx_, ry_, rz_, zm_, zp_);
    return;
  }

  ellipsoid::~ellipsoid()
  {
    return;
  }

  void ellipsoid::initialize(const datatools::properties & config_,
                             const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {
      double lunit = CLHEP::mm;
      if (config_.has_key ("length_unit")) {
        const std::string lunit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (lunit_str);
      }

      double x_radius;
      datatools::invalidate (x_radius);
      if (config_.has_key ("x_radius")) {
        x_radius = config_.fetch_real ("x_radius");
        if (! config_.has_explicit_unit ("x_radius")) {
          x_radius *= lunit;
        }
      } else if (config_.has_key ("x_diameter")) {
        x_radius = 0.5 * config_.fetch_real ("x_diameter");
        if (! config_.has_explicit_unit ("x_diameter")) {
          x_radius *= lunit;
        }
      }
      DT_THROW_IF (! datatools::is_valid (x_radius), std::logic_error,
                   "Missing elliptical_tube 'x_radius' property !");

      double y_radius;
      datatools::invalidate (y_radius);
      if (config_.has_key ("y_radius")) {
        y_radius = config_.fetch_real ("y_radius");
        if (! config_.has_explicit_unit ("y_radius")) {
          y_radius *= lunit;
        }
      } else if (config_.has_key ("y_diameter")) {
        y_radius = 0.5 * config_.fetch_real ("y_diameter");
        if (! config_.has_explicit_unit ("y_diameter")) {
          y_radius *= lunit;
        }
      }
      DT_THROW_IF (! datatools::is_valid (y_radius), std::logic_error,
                   "Missing elliptical_tube 'y_radius' property !");

      double z_radius;
      datatools::invalidate (z_radius);
      if (config_.has_key ("z_radius")) {
        z_radius = config_.fetch_real ("z_radius");
        if (! config_.has_explicit_unit ("z_radius")) {
          z_radius *= lunit;
        }
      } else if (config_.has_key ("x_diameter")) {
        z_radius = 0.5 * config_.fetch_real ("x_diameter");
        if (! config_.has_explicit_unit ("x_diameter")) {
          z_radius *= lunit;
        }
      }
      DT_THROW_IF (! datatools::is_valid (z_radius), std::logic_error,
                   "Missing elliptical_tube 'z_radius' property !");

      set(x_radius, y_radius, z_radius);

      double bottom_z;
      datatools::invalidate(bottom_z);
      if (config_.has_key ("bottom_z_cut")) {
        bottom_z = config_.fetch_real ("bottom_z_cut");
        if (! config_.has_explicit_unit ("bottom_z_cut")) {
          bottom_z *= lunit;
        }
	set_bottom_z_cut(bottom_z);
      }

      double top_z;
      datatools::invalidate(top_z);
      if (config_.has_key ("top_z_cut")) {
        top_z = config_.fetch_real ("top_z_cut");
        if (! config_.has_explicit_unit ("top_z_cut")) {
          top_z *= lunit;
        }
	set_top_z_cut(top_z);
      }
    }
    lock();
    return;
  }

  double
  ellipsoid::get_surface (uint32_t /*mask_*/) const
  {
    double s;
    datatools::invalidate(s);
    //    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return s;
  }

  double
  ellipsoid::get_volume (uint32_t /*flags*/) const
  {
    double v;
    datatools::invalidate(v);
    //    DT_THROW_IF(true, std::runtime_error, "Not implemented !");
    return v;
  }

  bool
  ellipsoid::is_valid() const
  {
    return datatools::is_valid(_x_radius_)
      && datatools::is_valid(_y_radius_)
      && datatools::is_valid(_z_radius_);
  }

  std::string
  ellipsoid::get_shape_name() const
  {
    return ellipsoid_label();
  }

  bool
  ellipsoid::is_inside (const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Ellipsoid is not valid !");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    double z0 = -_z_radius_;
    double z1 = +_z_radius_;
    if (datatools::is_valid(_bottom_z_cut_)) {
      z0 = _bottom_z_cut_;
    }
    if (datatools::is_valid(_top_z_cut_)) {
      z1 = _top_z_cut_;
    }
    double z = point_.z();
    if (z < z0 + hskin) {
      return false;
    }
    if (z > z1 - hskin) {
      return false;
    }
    double x = point_.x();
    double y = point_.y();
    double a = _x_radius_ - hskin;
    double b = _y_radius_ - hskin;
    double c = _z_radius_ - hskin;
    double term = gsl_pow_2(x / a) + gsl_pow_2(y / b) + gsl_pow_2(z / c);
    if (term <= 1.0) {
      return true;
    }
    /*
      double term = std::sqrt(1 - gsl_pow_2(z/_z_radius_));
      if (term > 1.e-7) {
      vector_3d ell_pos(point_.x(), point_.y());
      elliptical_sector ell_section(_x_radius_ * term - hskin, _y_radius_ * term - hskin);
      if (ell_section.is_on_surface(ell_pos, 0.0)) {
      return true;
      }
      }
    */
    return false;
  }

  bool
  ellipsoid::is_outside (const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Ellipsoid is not valid !");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    double z0 = -_z_radius_;
    double z1 = +_z_radius_;
    if (datatools::is_valid(_bottom_z_cut_)) {
      z0 = _bottom_z_cut_;
    }
    if (datatools::is_valid(_top_z_cut_)) {
      z1 = _top_z_cut_;
    }
    double z = point_.z();
    if (z < z0 - hskin) {
      return true;
    }
    if (z > z1 + hskin) {
      return true;
    }
    double x = point_.x();
    double y = point_.y();
    double a = _x_radius_ + hskin;
    double b = _y_radius_ + hskin;
    double c = _z_radius_ + hskin;
    double term = gsl_pow_2(x / a) + gsl_pow_2(y / b) + gsl_pow_2(z / c);
    if (term >= 1.0) {
      return true;
    }
    return false;
  }

  double
  ellipsoid::get_parameter (const std::string & flag_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Ellipsoid is not valid !");
    if  (flag_ == "x_radius")     return get_x_radius();
    if  (flag_ == "y_radius")     return get_y_radius();
    if  (flag_ == "z_radius")     return get_z_radius();
    if  (flag_ == "bottom_z_cut") return get_bottom_z_cut();
    if  (flag_ == "top_z_cut")    return get_top_z_cut();
    if  (flag_ == "surface")      return get_surface(FACE_ALL);
    if  (flag_ == "volume")       return get_volume();
    DT_THROW_IF(true, std::logic_error, "Unknown flag '" << flag_ << "' !");
  }

  vector_3d
  ellipsoid::get_normal_on_surface (const vector_3d & a_position,
                                    const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Ellipsoid is not valid !");
    vector_3d normal;
    invalidate(normal);
    switch (a_surface_bit.get_face_bits()) {
    case FACE_SIDE:
      normal.set(a_position.x() / _x_radius_,
                 a_position.y() / _y_radius_,
                 a_position.z() / _z_radius_);
      normal /= normal.mag();
      break;
    case FACE_BOTTOM:
      if (datatools::is_valid(_bottom_z_cut_)) normal.set(0.0, 0.0, -1.0);
      break;
    case FACE_TOP:
      if (datatools::is_valid(_top_z_cut_)) normal.set(0.0, 0.0, +1.0);
      break;
    }
    return normal;
  }

  void ellipsoid::compute_side_face(ellipsoid_sector & face_,
                                    placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Ellipsoid is not valid !");
    face_.reset();
    face_placement_.reset();
    face_.set_x_radius(_x_radius_);
    face_.set_y_radius(_y_radius_);
    face_.set_z_radius(_z_radius_);
    if (datatools::is_valid(_bottom_z_cut_)) {
      face_.set_bottom_z_cut(_bottom_z_cut_);
    }
    if (datatools::is_valid(_top_z_cut_)) {
      face_.set_top_z_cut(_top_z_cut_);
    }
    face_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    return;
  }

  void ellipsoid::compute_top_bottom_face(faces_mask_type face_id_,
                                          elliptical_sector & face_,
                                          placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Ellipsoid is not valid !");
    face_.reset();
    face_placement_.reset();
    switch(face_id_) {
    case FACE_BOTTOM:
      if (datatools::is_valid(_bottom_z_cut_)) {
        double term = std::sqrt(1 - gsl_pow_2(_bottom_z_cut_/ _z_radius_));
        face_.set_x_radius(_x_radius_ * term);
        face_.set_y_radius(_y_radius_ * term);
        face_placement_.set(0.0, 0.0, _bottom_z_cut_, 0.0, 0.0, 0.0);
      }
      break;
    case FACE_TOP:
      if (datatools::is_valid(_top_z_cut_)) {
        double term = std::sqrt(1 - gsl_pow_2(_top_z_cut_/ _z_radius_));
        face_.set_x_radius(_x_radius_ * term);
        face_.set_y_radius(_y_radius_ * term);
        face_placement_.set(0.0, 0.0, _top_z_cut_, 0.0, 0.0, 0.0);
      }
      break;
    default:
      break;
    }
    return;
  }

  unsigned int ellipsoid::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Ellipsoid is not valid !");
    unsigned int nfaces = 0;
    {
      // Side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      ellipsoid_sector & es = finfo.add_face<ellipsoid_sector>();
      compute_side_face(es, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("side");
      nfaces++;
    }
    if (datatools::is_valid(_bottom_z_cut_)) {
      // Bottom cut:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      elliptical_sector & es = finfo.add_face<elliptical_sector>();
      compute_top_bottom_face(FACE_BOTTOM, es, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_BOTTOM);
      finfo.set_identifier(fid);
      finfo.set_label("bottom");
      nfaces++;
    }
    if (datatools::is_valid(_top_z_cut_)) {
      // Top cut:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      elliptical_sector & es = finfo.add_face<elliptical_sector>();
      compute_top_bottom_face(FACE_TOP, es, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_TOP);
      finfo.set_identifier(fid);
      finfo.set_label("top");
      nfaces++;
    }
    return nfaces;
  }

  face_identifier ellipsoid::on_surface(const vector_3d & a_position,
                                        const face_identifier & a_surface_mask,
                                        double a_skin) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Ellipsoid is not valid !");
    double skin = get_skin(a_skin);

    face_identifier mask;
    if (a_surface_mask.is_valid()) {
      DT_THROW_IF(! a_surface_mask.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = a_surface_mask;
    } else {
      mask.set_face_bits_any();
    }

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(a_position, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    /*
      if (a_surface_mask.has_face_bit(FACE_SIDE)) {
      ellipsoid_sector side;
      placement side_placement;
      compute_side_face(side, side_placement);
      vector_3d p_side;
      side_placement.mother_to_child(a_point, p_side);
      if (side.is_on_surface(p_side, skin)) {
      return face_identifier(FACE_SIDE);
      }
      }

      if (datatools::is_valid(_top_z_cut_)) {
      if (a_surface_mask.has_face_bit(FACE_TOP)) {
      elliptical_sector top;
      placement top_placement;
      compute_top_bottom_face(FACE_TOP, top, top_placement);
      vector_3d p_top;
      top_placement.mother_to_child(a_point, p_top);
      if (top.is_on_surface(p_top, skin)) {
      return face_identifier(FACE_TOP);
      }
      }
      }

      if (datatools::is_valid(_bottom_z_cut_)) {
      if (a_surface_mask.has_face_bit(FACE_BOTTOM)) {
      elliptical_sector bottom;
      placement bottom_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom, bottom_placement);
      vector_3d p_bottom;
      bottom_placement.mother_to_child(a_point, p_bottom);
      if (bottom.is_on_surface(p_bottom, skin)) {
      return face_identifier(FACE_BOTTOM);
      }
      }
      }
    */

    return face_identifier::face_invalid();
  }

  bool
  ellipsoid::find_intercept (const vector_3d & from_,
                             const vector_3d & direction_,
                             face_intercept_info & intercept_,
                             double skin_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Not valid !");
    intercept_.reset();

    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 3;
    face_intercept_info intercepts[NFACES];
    unsigned int candidate_impact_counter = 0;

    int face_counter = 0;
    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (!finfo.is_valid()) {
        continue;
      }
      const i_shape_2d & face = finfo.get_face_ref();
      const placement & face_placement = finfo.get_positioning();
      const face_identifier & face_id = finfo.get_identifier();
      if (face.i_find_intercept::find_intercept(from_,
                                                direction_,
                                                face_placement,
                                                intercepts[face_counter],
                                                skin)) {
        intercepts[face_counter].set_face_id(face_id);
        candidate_impact_counter++;
      }
      face_counter++;
    }

    /*
      {
      const int FACE_INDEX = 0;
      ellipsoid_sector side_face;
      placement side_face_placement;
      compute_side_face(side_face, side_face_placement);
      if (side_face.i_find_intercept::find_intercept(from_,
      direction_,
      side_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_SIDE);
      candidate_impact_counter++;
      }
      }

      if (datatools::is_valid(_bottom_z_cut_)) {
      const int FACE_INDEX = 1;
      elliptical_sector bottom_face;
      placement bottom_face_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom_face, bottom_face_placement);
      if (bottom_face.i_find_intercept::find_intercept(from_,
      direction_,
      bottom_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_BOTTOM);
      candidate_impact_counter++;
      }
      }

      if (datatools::is_valid(_top_z_cut_)) {
      const int FACE_INDEX = 2;
      elliptical_sector top_face;
      placement top_face_placement;
      compute_top_bottom_face(FACE_TOP, top_face, top_face_placement);
      if (top_face.i_find_intercept::find_intercept(from_,
      direction_,
      top_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_TOP);
      candidate_impact_counter++;
      }
      }
    */

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.set_face_id(intercepts[iface].get_face_id());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return intercept_.is_ok();
  }

  std::ostream &
  operator<< (std::ostream & out_, const ellipsoid & e_)
  {
    out_ << '{' << ellipsoid::ellipsoid_label() << ' '
         << e_._x_radius_ << ' '
         << e_._y_radius_ << ' '
         << e_._z_radius_ << ' '
         << e_._bottom_z_cut_ << ' '
         << e_._top_z_cut_ << '}';
    return out_;
  }

  std::istream &
  operator>> (std::istream & in_, ellipsoid & e_)
  {
    e_.reset();
    char c = 0;
    in_.get (c);
    if (c != '{') {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != ellipsoid::ellipsoid_label()) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double rx, ry, rz;
    in_ >> rx >> ry >> rz;
    if (! in_) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    double zbot, ztop;
    in_ >> zbot >> ztop;
    if (! in_) {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    c = 0;
    in_.get (c);
    if (c != '}')  {
      in_.clear (std::ios_base::failbit);
      return in_;
    }
    try {
      e_.set(rx, ry, rz, zbot, ztop);
    }
    catch (...) {
      e_.reset();
      in_.clear (std::ios_base::failbit);
    }
    return in_;
  }

  void ellipsoid::tree_dump (std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X radius : " << get_x_radius () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y radius : " << get_y_radius () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z radius : " << get_z_radius () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z bottom-cut : " << get_bottom_z_cut () / CLHEP::mm << " mm" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z top-cut    : " << get_top_z_cut () / CLHEP::mm << " mm" << std::endl;

    return;
  }

  void ellipsoid::generate_wires_self(wires_type & wires_,
                                      uint32_t options_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Ellipsoid is not valid !");
    bool draw_side_face   = !(options_ & WR_ELLIPSOID_NO_SIDE_FACE);
    bool draw_bottom_face = !(options_ & WR_ELLIPSOID_NO_BOTTOM_FACE);
    bool draw_top_face    = !(options_ & WR_ELLIPSOID_NO_TOP_FACE);
    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    if (draw_side_face) {
      uint32_t options = base_options;
      ellipsoid_sector side;
      placement side_placement;
      compute_side_face(side, side_placement);
      side.generate_wires(wires_, side_placement, options);
    }

    if (datatools::is_valid(_top_z_cut_)) {
      if (draw_top_face) {
        uint32_t options = base_options;
        elliptical_sector top;
        placement top_placement;
        compute_top_bottom_face(FACE_TOP, top, top_placement);
        top.generate_wires(wires_, top_placement, options);
      }
    }

    if (datatools::is_valid(_bottom_z_cut_)) {
      if (draw_bottom_face) {
        uint32_t options = base_options;
        elliptical_sector bottom;
        placement bottom_placement;
        compute_top_bottom_face(FACE_BOTTOM, bottom, bottom_placement);
        bottom.generate_wires(wires_, bottom_placement, options);
      }
    }

    return;
  }

  // static
  void ellipsoid::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default \n"
                              "length unit for length parameters of the box.  \n"
                              )
        .add_example("Set the default length unit:        \n"
                     "                                    \n"
                     "  length_unit : string = \"mm\"     \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("x_radius")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The x radius of the ellipsoid")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the x radius:                     \n"
                     "                                      \n"
                     "  x_radius : real as length = 2.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y_radius")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The y radius of the ellipsoid")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the y radius:                     \n"
                     "                                      \n"
                     "  y_radius : real as length = 4.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z_radius")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The z radius of the ellipsoid")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the z radius:                     \n"
                     "                                      \n"
                     "  z_radius : real as length = 6.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z_top_cut")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The z top cut")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the z top cut:                        \n"
                     "                                          \n"
                     "  top_z_cut : real as length = 1.0 cm     \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z_bottom_cut")
        .set_from("geomtools::ellipsoid")
        .set_terse_description("The z bottom cut")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the z bottom cut:                    \n"
                     "                                          \n"
                     "  bottom_z_cut : real as length = 1.5 cm  \n"
                     "                                          \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::ellipsoid the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::ellipsoid, ocd_)
{
  // The class name :
  ocd_.set_class_name ("geomtools::ellipsoid");

  // The class terse description :
  ocd_.set_class_description ("A class representing a 3D ellipsoid solid shape");

  // The library the class belongs to :
  ocd_.set_class_library ("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The ellipsoid solid 3D-shape");

  // Populate the specific OCD :
  geomtools::ellipsoid::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the  \n"
                               "``datatools::properties`` ASCII format::     \n"
                               "                                             \n"
                               "  length_unit      : string = \"mm\"         \n"
                               "  tolerance.length : real   = 1.0e-6 um      \n"
                               "  x_radius : real as length = 200.0 mm       \n"
                               "  y_radius : real as length = 100.0 mm       \n"
                               "  z_radius : real as length = 600.0 mm       \n"
                               "  z_top_cut        : real   =  0.0 cm        \n"
                               "  z_bottom_cut     : real   = 30.0 cm        \n"
                               "                                             \n"
                               );

  /** Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides (see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'geomtools::ellipsoid' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::ellipsoid, "geomtools::ellipsoid")
