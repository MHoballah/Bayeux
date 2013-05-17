// -*- mode: c++ ; -*-
/* sphere_cut.cc
 */

#include <sphere_cut.h>

#include <stdexcept>
#include <sstream>

#include <data.h>

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(sphere_cut, "sphere_cut");

void sphere_cut::set_radius(double radius_)
{
  _radius_ = radius_;
  return;
}

void sphere_cut::set_center(double x0_, double y0_, double z0_)
{
  _x0_ = x0_;
  _y0_ = y0_;
  _z0_ = z0_;
  return;
}

void sphere_cut::set_reversed(bool reversed_)
{
  _reversed_ = reversed_;
  return;
}

// ctor:
CUT_CONSTRUCTOR_IMPLEMENT_HEAD(sphere_cut, a_logging_priority)
{
  _radius_ = 1.0;
  _x0_ = 0.0;
  _y0_ = 0.0;
  _z0_ = 0.0;
  _reversed_ = false;
  return;
}

CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(sphere_cut)

// static method used within a cut factory:
CUT_INITIALIZE_IMPLEMENT_HEAD(sphere_cut,
                              a_configuration,
                              a_service_manager,
                              a_cut_dict)
{
  DT_THROW_IF(is_initialized(),
              std::logic_error,
              "Sphere cut named '" << (has_name()? get_name(): "?")
              << "' is already initialized !");
  _common_initialize(a_configuration);

  double radius = 1.0;
  double center[3];
  bool reversed = false;
  center[0] = 0.0;
  center[1] = 0.0;
  center[2] = 0.0;

  if (a_configuration.has_key("center")) {
    std::vector<double> v;
    a_configuration.fetch("center", v);
    DT_THROW_IF(v.size() != 3,
                std::range_error,
                "Invalid vector size for center !");
    center[0] = v[0];
    center[1] = v[1];
    center[2] = v[2];
  }

  if (a_configuration.has_key("radius")) {
    radius = a_configuration.fetch_real("radius");
    DT_THROW_IF(radius <= 0.0,
                std::domain_error,
                "Invalid radius !");
  }

  if (a_configuration.has_flag("reversed")) {
    reversed = true;
  }

  this->set_radius(radius);
  this->set_center(center[0], center[1], center[2]);
  this->set_reversed(reversed);

  _set_initialized(true);
  return;
}

CUT_RESET_IMPLEMENT_HEAD(sphere_cut)
{
  _set_initialized(false);
  _radius_ = 1.0;
  _x0_ = 0.0;
  _y0_ = 0.0;
  _z0_ = 0.0;
  _reversed_ = false;
  this->i_cut::_reset();
  return;
}

CUT_ACCEPT_IMPLEMENT_HEAD(sphere_cut)
{
  DT_THROW_IF(! is_initialized(),
              std::logic_error,
              "Sphere cut named '" << (has_name()? get_name(): "?")
              << "' is not initialized !");
  const data & a_data = get_user_data<data>();
  double hx = a_data.x - _x0_;
  double hy = a_data.y - _y0_;
  double hz = a_data.z - _z0_;
  double r2 =(hx * hx + hy * hy + hz * hz);
  double rlim2 = _radius_ * _radius_;
  bool result = cuts::SELECTION_ACCEPTED;
  if (r2 > rlim2) {
    result = cuts::SELECTION_REJECTED;
  }
  if (_reversed_) {
    result = (result == cuts::SELECTION_REJECTED)? cuts::SELECTION_ACCEPTED: cuts::SELECTION_REJECTED;
  }
  return result;
}

// end of sphere_cut.cc
