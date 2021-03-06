// -*- mode: c++ ; -*-
// visibility.cc

// Ourselves:
#include <geomtools/visibility.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

namespace geomtools {

  using namespace std;

  // static
  const visibility::constants &
  visibility::constants::instance()
  {
    static boost::scoped_ptr<visibility::constants> g_global_constants(0);
    if( g_global_constants.get() == 0) {
      g_global_constants.reset(new visibility::constants);
    }
    return *g_global_constants.get();
  }

  visibility::constants::constants()
  {
    VISIBILITY_PREFIX                  = "visibility.";
    VISIBILITY_HIDDEN_FLAG             = "hidden";
    VISIBILITY_HIDDEN_ENVELOPE_FLAG    = "hidden_envelope";
    VISIBILITY_DAUGHTERS_HIDDEN_FLAG   = "daughters.hidden";
    VISIBILITY_COLOR_PROPERTY          = "color";
    VISIBILITY_ENVELOPE_COLOR_PROPERTY = "envelope_color";
    VISIBILITY_WIRED_CYLINDER_FLAG     = "wired_cylinder";
    VISIBILITY_OPACITY_PROPERTY        = "opacity";
    VISIBILITY_OPACITY_RULE_PROPERTY   = "opacity_rule";
    VISIBILITY_VISIBLE_EDGES_FLAG      = "visible_edges";
    VISIBILITY_DEFAULT_OPACITY         = 0.2;
    return;
  }

  string visibility::make_key(const string & key_)
  {
    ostringstream key_oss;
    key_oss << visibility::constants::instance().VISIBILITY_PREFIX << key_;
    return key_oss.str();
  }

  void visibility::extract(const datatools::properties & source_,
                           datatools::properties & target_)
  {
    source_.export_starting_with(target_, visibility::constants::instance().VISIBILITY_PREFIX);
    return;
  }

  bool visibility::has_flag(const datatools::properties & config_,
                            const string & flag_)
  {
    return(config_.has_flag(visibility::make_key(flag_)));
  }

  bool visibility::has_key(const datatools::properties & config_,
                           const string & key_)
  {
    return(config_.has_key(visibility::make_key(key_)));
  }

  bool visibility::is_shown(const datatools::properties & config_)
  {
    bool shown = true; // default:
    if(visibility::has_flag(config_, visibility::constants::instance().VISIBILITY_HIDDEN_FLAG)) {
      shown = false;
    }
    return shown;
  }

  bool visibility::is_hidden(const datatools::properties & config_)
  {
    return ! visibility::is_shown(config_);
  }

  bool visibility::is_hidden_envelope(const datatools::properties & config_)
  {
    return visibility::has_flag(config_, visibility::constants::instance().VISIBILITY_HIDDEN_ENVELOPE_FLAG);
  }

  bool visibility::is_daughters_shown(const datatools::properties & config_)
  {
    return ! visibility::is_daughters_hidden(config_);
  }

  bool visibility::is_daughters_hidden(const datatools::properties & config_)
  {
    return visibility::has_flag(config_, visibility::constants::instance().VISIBILITY_DAUGHTERS_HIDDEN_FLAG);
  }

  bool visibility::is_visible_edges(const datatools::properties & config_)
  {
    return visibility::has_flag(config_, visibility::constants::instance().VISIBILITY_VISIBLE_EDGES_FLAG);
  }

  bool visibility::validate_opacity(double opacity_)
  {
    return datatools::is_valid(opacity_) && opacity_ >= 0.0 && opacity_ <= 1.0;
  }

  bool visibility::has_opacity(const datatools::properties & config_)
  {
    return visibility::has_key(config_, visibility::constants::instance().VISIBILITY_OPACITY_PROPERTY);
  }

  double visibility::default_opacity()
  {
    return visibility::constants::instance().VISIBILITY_DEFAULT_OPACITY;
  }

  double visibility::get_opacity(const datatools::properties & config_)
  {
    if(! has_opacity(config_)) return 1.0;
    return config_.fetch_real(visibility::make_key(visibility::constants::instance().VISIBILITY_OPACITY_PROPERTY));
  }

  void visibility::set_opacity(datatools::properties & config_, double opacity_)
  {
    config_.update_real(visibility::make_key(visibility::constants::instance().VISIBILITY_OPACITY_PROPERTY), opacity_);
    return;
  }

  bool visibility::has_color(const datatools::properties & config_)
  {
    return visibility::has_key(config_, visibility::constants::instance().VISIBILITY_COLOR_PROPERTY);
  }

  string visibility::get_color(const datatools::properties & config_)
  {
    if(! has_color(config_)) return "";
    return config_.fetch_string(visibility::make_key(visibility::constants::instance().VISIBILITY_COLOR_PROPERTY));
  }

  void visibility::set_color(datatools::properties & config_,
                             const string & color_)
  {
    config_.update(visibility::make_key(visibility::constants::instance().VISIBILITY_COLOR_PROPERTY), color_);
    return;
  }

  bool visibility::has_envelope_color(const datatools::properties & config_)
  {
    return visibility::has_key(config_, visibility::constants::instance().VISIBILITY_ENVELOPE_COLOR_PROPERTY);
  }

  string visibility::get_envelope_color(const datatools::properties & config_)
  {
    if(! has_envelope_color(config_)) return "";
    return config_.fetch_string(visibility::make_key(visibility::constants::instance().VISIBILITY_ENVELOPE_COLOR_PROPERTY));
  }

  void visibility::set_envelope_color(datatools::properties & config_,
                                      const string & envelope_color_)
  {
    config_.update(visibility::make_key(visibility::constants::instance().VISIBILITY_ENVELOPE_COLOR_PROPERTY), envelope_color_);
    return;
  }

  bool visibility::is_wired_cylinder(const datatools::properties & config_)
  {
    return visibility::has_flag(config_, visibility::constants::instance().VISIBILITY_WIRED_CYLINDER_FLAG);
  }

} // end of namespace geomtools
