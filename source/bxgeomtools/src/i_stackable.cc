// i_stackable.cc

// Ourselves:
#include <geomtools/i_stackable.h>

// Standard Library:
#include <sstream>
#include <stdexcept>

// This project:
#include <datatools/clhep_units.h>
#include <datatools/units.h>

namespace geomtools {

  using namespace std;

  const std::string stackable::STACKABLE_PREFIX = "stackable.";
  const std::string stackable::STACKABLE_LENGTH_UNIT_PROPERTY = "length_unit";
  const std::string stackable::STACKABLE_XMIN_PROPERTY = "xmin";
  const std::string stackable::STACKABLE_XMAX_PROPERTY = "xmax";
  const std::string stackable::STACKABLE_YMIN_PROPERTY = "ymin";
  const std::string stackable::STACKABLE_YMAX_PROPERTY = "ymax";
  const std::string stackable::STACKABLE_ZMIN_PROPERTY = "zmin";
  const std::string stackable::STACKABLE_ZMAX_PROPERTY = "zmax";
  const std::string stackable::STACKABLE_PLAY_PROPERTY = "play";
  const std::string stackable::STACKABLE_LIMITS_PROPERTY = "limits";

  i_stackable::i_stackable()
  {
    return;
  }

  i_stackable::~i_stackable()
  {
    return;
  }

  bool i_stackable::has_xmin() const
  {
    return datatools::is_valid(get_xmin ());
  }

  bool i_stackable::has_xmax() const
  {
    return datatools::is_valid(get_xmax ());
  }

  bool i_stackable::has_ymin() const
  {
    return datatools::is_valid(get_ymin ());
  }

  bool i_stackable::has_ymax() const
  {
    return datatools::is_valid(get_ymax ());
  }

  bool i_stackable::has_zmin() const
  {
    return datatools::is_valid(get_zmin ());
  }

  bool i_stackable::has_zmax() const
  {
    return datatools::is_valid(get_zmax ());
  }

  std::string stackable::make_key (const string & key_)
  {
    std::ostringstream key_oss;
    key_oss << stackable::STACKABLE_PREFIX << key_;
    return key_oss.str ();
  }

  void stackable::extract (const datatools::properties & source_,
                           datatools::properties & target_)
  {
    source_.export_starting_with (target_, stackable::STACKABLE_PREFIX);
    return;
  }

  bool stackable::has_xmin (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY));
  }

  bool stackable::has_xmax (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY));
  }

  bool stackable::has_ymin (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY));
  }

  bool stackable::has_ymax (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY));
  }

  bool stackable::has_zmin (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY));
  }

  bool stackable::has_zmax (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY));
  }

  double stackable::get_xmin (const datatools::properties & source_, double length_unit_)
  {
    static const std::string xmin_key = stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY);
    double xmin = source_.fetch_real (xmin_key);
    if (length_unit_ > 0.0 && source_.has_explicit_unit(xmin_key)) xmin *= length_unit_;
    return xmin;
  }

  double stackable::get_xmax (const datatools::properties & source_, double length_unit_)
  {
    static const std::string xmax_key = stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY);
    double xmax = source_.fetch_real (xmax_key);
    if (length_unit_ > 0.0 && source_.has_explicit_unit(xmax_key)) xmax *= length_unit_;
    return xmax;
  }

  double stackable::get_ymin (const datatools::properties & source_, double length_unit_)
  {
    static const std::string ymin_key = stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY);
    double ymin = source_.fetch_real (ymin_key);
    if (length_unit_ > 0.0 && source_.has_explicit_unit(ymin_key)) ymin *= length_unit_;
    return ymin;
  }

  double stackable::get_ymax (const datatools::properties & source_, double length_unit_)
  {
    static const std::string ymax_key = stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY);
    double ymax = source_.fetch_real (ymax_key);
    if (length_unit_ > 0.0 && source_.has_explicit_unit(ymax_key)) ymax *= length_unit_;
    return ymax;
  }

  double stackable::get_zmin (const datatools::properties & source_, double length_unit_)
  {
    static const std::string zmin_key = stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY);
    double zmin = source_.fetch_real (zmin_key);
    if (length_unit_ > 0.0 && source_.has_explicit_unit(zmin_key)) zmin *= length_unit_;
    return zmin;
  }

  double stackable::get_zmax (const datatools::properties & source_, double length_unit_)
  {
    static const std::string zmax_key = stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY);
    double zmax = source_.fetch_real (zmax_key);
    if (length_unit_ > 0.0 && source_.has_explicit_unit(zmax_key)) zmax *= length_unit_;
    return zmax;
  }

  void stackable::set_xmin (datatools::properties & target_,
                            double xmin_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY),
                   xmin_);
    return;
  }

  void stackable::set_xmax (datatools::properties & target_,
                            double xmax_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY),
                   xmax_);
    return;
  }

  void stackable::set_ymin (datatools::properties & target_,
                            double ymin_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY),
                   ymin_);
    return;
  }

  void stackable::set_ymax (datatools::properties & target_,
                            double ymax_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY),
                   ymax_);
    return;
  }

  void stackable::set_zmin (datatools::properties & target_,
                            double zmin_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY),
                   zmin_);
    return;
  }

  void stackable::set_zmax (datatools::properties & target_,
                            double zmax_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY),
                   zmax_);
    return;
  }

  void stackable::unset_xmin (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY));
    return;
  }

  void stackable::unset_xmax (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY));
    return;
  }

  void stackable::unset_ymin (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY));
    return;
  }

  void stackable::unset_ymax (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY));
    return;
  }

  void stackable::unset_zmin (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY));
    return;
  }

  void stackable::unset_zmax (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY));
    return;
  }

  void stackable::unset (datatools::properties & target_)
  {
    unset_xmin (target_);
    unset_xmax (target_);
    unset_ymin (target_);
    unset_xmax (target_);
    unset_zmin (target_);
    unset_zmax (target_);
    return;
  }

  void stackable::set (datatools::properties & target_,
                       const i_stackable & sd_)
  {
    set_xmin (target_, sd_.get_xmin ());
    set_xmax (target_, sd_.get_xmax ());
    set_ymin (target_, sd_.get_ymin ());
    set_xmax (target_, sd_.get_ymax ());
    set_zmin (target_, sd_.get_zmin ());
    set_zmax (target_, sd_.get_zmax ());
    return;
  }

  bool stackable_data::initialize (const datatools::properties & config_)
  {
    double lunit = CLHEP::mm;

    std::string length_unit_key = stackable::make_key (stackable::STACKABLE_LENGTH_UNIT_PROPERTY);
    if (config_.has_key (length_unit_key)) {
      std::string length_unit_str = config_.fetch_string (length_unit_key);
      lunit = datatools::units::get_length_unit_from (length_unit_str);
    }

    static const std::string stackable_limits_key
      = stackable::make_key(stackable::STACKABLE_LIMITS_PROPERTY);
    if (config_.has_key (stackable_limits_key)) {
      std::vector<double> limits;
      config_.fetch(stackable_limits_key, limits);
      DT_THROW_IF (limits.size () != 6, std::logic_error,
                   "Stacking limits vector should provide 6 dimensions !");
      if (! config_.has_explicit_unit (stackable_limits_key)) {
        limits[0] *= lunit;
        limits[2] *= lunit;
        limits[4] *= lunit;
        limits[1] *= lunit;
        limits[3] *= lunit;
        limits[5] *= lunit;
      }
      xmin = limits[0];
      ymin = limits[2];
      zmin = limits[4];
      xmax = limits[1];
      ymax = limits[3];
      zmax = limits[5];
    } else {
      std::string xmin_key = stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY);
      if (config_.has_key (xmin_key)) {
        xmin = config_.fetch_real (xmin_key);
        if (! config_.has_explicit_unit (xmin_key)) xmin *= lunit;
      }

      std::string xmax_key = stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY);
      if (config_.has_key (xmax_key)) {
        xmax = config_.fetch_real (xmax_key);
        if (! config_.has_explicit_unit (xmax_key)) xmax *= lunit;
      }

      std::string ymin_key = stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY);
      if (config_.has_key (ymin_key)) {
        ymin = config_.fetch_real (ymin_key);
        if (! config_.has_explicit_unit (ymin_key)) ymin *= lunit;
      }

      std::string ymax_key = stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY);
      if (config_.has_key (ymax_key)) {
        ymax = config_.fetch_real (ymax_key);
        if (! config_.has_explicit_unit (ymax_key)) ymax *= lunit;
      }

      std::string zmin_key = stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY);
      if (config_.has_key (zmin_key)) {
        zmin = config_.fetch_real (zmin_key);
        if (! config_.has_explicit_unit (zmin_key)) zmin *= lunit;
      }

      std::string zmax_key = stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY);
      if (config_.has_key (zmax_key)) {
        zmax = config_.fetch_real (zmax_key);
        if (! config_.has_explicit_unit (zmax_key)) zmax *= lunit;
      }
    }

    // return is_valid ();
    return is_valid_very_weak();
  }

  bool stackable_data::check(const stackable::stackability_mode flag_) const
  {
    switch (flag_) {
    case stackable::STACKABILITY_XMIN:
      return this->has_xmin();
    case stackable::STACKABILITY_XMAX:
      return this->has_xmax();
    case stackable::STACKABILITY_YMIN:
      return this->has_ymin();
    case stackable::STACKABILITY_YMAX:
      return this->has_ymax();
    case stackable::STACKABILITY_ZMIN:
      return this->has_zmin();
    case stackable::STACKABILITY_ZMAX:
      return this->has_zmax();
    case stackable::STACKABILITY_X:
      return this->is_valid_x();
    case stackable::STACKABILITY_Y:
      return this->is_valid_y();
    case stackable::STACKABILITY_Z:
      return this->is_valid_z();
    case stackable::STACKABILITY_XY:
      return this->is_valid_x() && this->is_valid_y();
    case stackable::STACKABILITY_YZ:
      return this->is_valid_y() && this->is_valid_z();
    case stackable::STACKABILITY_XZ:
      return this->is_valid_x() && this->is_valid_z();
    case stackable::STACKABILITY_VERY_WEAK:
      return this->is_valid_very_weak();
    case stackable::STACKABILITY_WEAK:
      return this->is_valid_weak();
    case stackable::STACKABILITY_STRONG:
      return this->is_valid();
    default:
      break;
    }
    return ! this->is_valid_very_weak();
  }

  bool stackable_data::is_valid_x () const
  {
    return has_xmin() && has_xmax();
  }

  bool stackable_data::is_valid_y () const
  {
    return has_ymin() && has_ymax();
  }

  bool stackable_data::is_valid_z () const
  {
    return has_zmin() && has_zmax();
  }

  bool stackable_data::is_valid () const
  {
    return is_valid_x () && is_valid_y () && is_valid_z ();
  }

  bool stackable_data::is_valid_by_axis(axis_type axis_) const
  {
    DT_THROW_IF(axis_ == AXIS_INVALID, std::logic_error, "Invalid axis!");
    switch (axis_) {
    case AXIS_X: return is_valid_x();
    case AXIS_Y: return is_valid_y();
    case AXIS_Z: return is_valid_z();
    default:  break;
    }
    return false;
  }

  bool stackable_data::is_valid_weak () const
  {
    return is_valid_x()
      || is_valid_y()
      || is_valid_z();
  }

  bool stackable_data::is_valid_very_weak () const
  {
    return datatools::is_valid(xmin)
      || datatools::is_valid(xmax)
      || datatools::is_valid(ymin)
      || datatools::is_valid(ymax)
      || datatools::is_valid(zmin)
      || datatools::is_valid(zmax);
  }

  void stackable_data::invalidate ()
  {
    datatools::invalidate(xmin);
    datatools::invalidate(xmax);
    datatools::invalidate(ymin);
    datatools::invalidate(ymax);
    datatools::invalidate(zmin);
    datatools::invalidate(zmax);
    return;
  }

  void stackable_data::reset()
  {
    invalidate();
    return;
  }

  stackable_data::stackable_data ()
  {
    invalidate();
  }

  stackable_data::~stackable_data() {
  }

  double stackable_data::get_xmin() const
  {
    return xmin;
  }

  double stackable_data::get_xmax() const
  {
    return xmax;
  }

  double stackable_data::get_ymin() const
  {
    return ymin;
  }

  double stackable_data::get_ymax() const
  {
    return ymax;
  }

  double stackable_data::get_zmin() const
  {
    return zmin;
  }

  double stackable_data::get_zmax() const
  {
    return zmax;
  }

  void stackable_data::tree_dump (std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }
    std::ostringstream xyz_out;
    if (is_valid_x()) {
      out_ << indent << datatools::i_tree_dumpable::tag << "xmin = " << xmin / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "xmax = " << xmax / CLHEP::mm << " mm" << std::endl;
      xyz_out << "X";
    }
    if (is_valid_y()) {
      out_ << indent << datatools::i_tree_dumpable::tag << "ymin = " << ymin / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "ymax = " << ymax / CLHEP::mm << " mm" << std::endl;
      xyz_out << "Y";
    }
    if (is_valid_z()) {
      out_ << indent << datatools::i_tree_dumpable::tag << "zmin = " << zmin / CLHEP::mm << " mm" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "zmax = " << zmax / CLHEP::mm << " mm" << std::endl;
      xyz_out << "Z";
    }
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Valid for axis: '" << xyz_out.str() << "'" << std::endl;
    return;
  }

  void stackable_data::dump (std::ostream & out_) const
  {
    // tree_dump (out_, "geomtools::stackable_data: ");
    out_ << "|-- " << "xmin = " << xmin / CLHEP::mm << " mm" << std::endl;
    out_ << "|-- " << "xmax = " << xmax / CLHEP::mm << " mm" << std::endl;
    out_ << "|-- " << "ymin = " << ymin / CLHEP::mm << " mm" << std::endl;
    out_ << "|-- " << "ymax = " << ymax / CLHEP::mm << " mm" << std::endl;
    out_ << "|-- " << "zmin = " << zmin / CLHEP::mm << " mm" << std::endl;
    out_ << "`-- " << "zmax = " << zmax / CLHEP::mm << " mm" << std::endl;
    return;
  }

} // end of namespace geomtools
