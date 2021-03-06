/* from_file_vg.cc
 */

// Ourselves:
#include <genvtx/from_file_vg.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>

// Third party:
// - Boost
#include <boost/filesystem.hpp>
// - Bayeux/datatools
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>

namespace genvtx {

  using namespace std;

  GENVTX_VG_REGISTRATION_IMPLEMENT(from_file_vg,"genvtx::from_file_vg")

  double from_file_vg::default_length_unit()
  {
    static double val = 1.0;
    return val;
  }

  double from_file_vg::default_time_unit()
  {
    static double val = 1.0;
    return val;
  }

  bool from_file_vg::is_open  () const
  {
    return _open_;
  }

  const string & from_file_vg::get_filename () const
  {
    return _filename_;
  }

  void from_file_vg::set_filename (const string & filename_)
  {
    DT_THROW_IF (is_initialized(), logic_error, "Already initialized !");
    DT_THROW_IF (_open_, logic_error, "Cannot change source file name as source file is already opened !");
    _filename_ = filename_;
    return;
  }

  void from_file_vg::_open_source ()
  {
    DT_THROW_IF (_open_, logic_error, "Source file is already opened !");
    DT_THROW_IF (_filename_.empty (), logic_error, "Missing source file name !");

    datatools::fetch_path_with_env (_filename_);

    DT_THROW_IF (! boost::filesystem::exists (_filename_),
                 runtime_error,"Source file '" << _filename_ << "' does not exist !");
    _source_.open (_filename_.c_str ());
    DT_THROW_IF (! _source_,logic_error,
                 "Cannot open source file '" << _filename_ << "' !");
    _open_ = true;
    _read_next ();
    return;
  }

  void from_file_vg::_close_source ()
  {
    DT_THROW_IF (! _open_, logic_error, "Source file is not opened !");
    _source_.close ();
    _filename_ = "";
    geomtools::invalidate (_next_);
    _open_ = true;
    return;
  }

  void from_file_vg::set_length_unit (double lu_)
  {
     DT_THROW_IF (lu_ <= 0.0, std::logic_error, "Invalid length unit !");
    _length_unit_ = lu_;
    return;
  }

  double from_file_vg::get_length_unit () const
  {
    return _length_unit_;
  }

  void from_file_vg::set_time_unit (double tu_)
  {
     DT_THROW_IF (tu_ <= 0.0, std::logic_error, "Invalid time unit !");
    _time_unit_ = tu_;
    return;
  }

  double from_file_vg::get_time_unit () const
  {
    return _time_unit_;
  }

  bool from_file_vg::is_initialized () const
  {
    return is_open();
  }

  void from_file_vg::initialize (const ::datatools::properties & configuration_,
                                 ::datatools::service_manager & /*service_manager_*/,
                                 ::genvtx::vg_dict_type & /*vgens_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

    // Fech parameters:

    if (configuration_.has_key("time_generator")) {
      bool time_gen = configuration_.fetch_boolean("time_generator");
      _set_time_generator(time_gen);
    }

    if (_filename_.empty()) {
      if (configuration_.has_key("filename")) {
        std::string filename = configuration_.fetch_string("filename");
        set_filename(filename);
      }
    }

    if (!datatools::is_valid(_length_unit_)) {
      if (configuration_.has_key("length_unit")) {
        std::string  lunit_str = configuration_.fetch_string("length_unit");
        double lunit = datatools::units::get_length_unit_from(lunit_str);
        set_length_unit(lunit);
      }
    }
    if (! datatools::is_valid(_length_unit_)) {
      set_length_unit(default_length_unit());
    }

    if (is_time_generator()) {
      if (!datatools::is_valid(_time_unit_)) {
        if (configuration_.has_key("time_unit")) {
          std::string tunit_str = configuration_.fetch_string("time_unit");
          double tunit = datatools::units::get_time_unit_from(tunit_str);
          set_time_unit(tunit);
        }
      }
      if (! datatools::is_valid(_time_unit_)) {
        set_time_unit(default_time_unit());
      }
    }

    _open_source();
    return;
  }

  void from_file_vg::reset()
  {
    if (_open_) {
      _close_source();
    }
    return;
  }

  from_file_vg::from_file_vg() : genvtx::i_vertex_generator()
  {
    _filename_ = "";
    _open_ = false;
    geomtools::invalidate(_next_);
    datatools::invalidate(_next_time_);
    datatools::invalidate(_length_unit_);
    datatools::invalidate(_time_unit_);
    return;
  }

  from_file_vg::~from_file_vg()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void from_file_vg::_read_next()
  {
    DT_THROW_IF (! _open_, logic_error, "Source file is not opened !");
    double x, y, z;
    double t;
    geomtools::invalidate(_next_);
    datatools::invalidate(_next_time_);

    bool goon = true;
    while (goon) {
      goon = false;
      string line;
      std::getline (_source_, line);
      // detect special cases:
      {
        istringstream iss (line);
        string word;
        iss >> word;
        // Skip blank lines:
        if (word.empty ()) {
          goon = true;
        }
        // Skip commented line:
        else if (word[0] == '#') {
          if (word.substr(0, 14) == "#@length_unit=") {
            std::istringstream lunit_iss(word.substr(14));
            // std::cerr << "DEVEL: word.substr(14)='" << word.substr(14) << "'\n";
            std::string lunit_str;
            lunit_iss >> lunit_str >> std::ws;
            double length_unit =
              datatools::units::get_length_unit_from(lunit_str);
            set_length_unit(length_unit);
          } else if (word.substr(0, 12) == "#@time_unit=") {
            std::istringstream tunit_iss(word.substr(12));
            std::string tunit_str;
            tunit_iss >> tunit_str >> std::ws;
            double time_unit =
              datatools::units::get_time_unit_from(tunit_str);
            _set_time_generator(true);
            set_time_unit(time_unit);
          }
          goon = true;
          continue;
        }
      }
      if (! goon) {
        std::istringstream iss (line);
        iss >> x >> y >> z >> std::ws;
        // Only throw if stream is bad and NOT at eof. Some c++ libraries
        // (Apple clang on Mavericks) will set bad/failbit at eof. Seems
        // not to be C++11 standard compliant, but is still present!
        DT_THROW_IF (! iss && !iss.eof(),
                     logic_error,
                     "'x y z' format error at invalid line '" << line << "' !");
        _next_.set (x, y, z);
        if (is_time_generator()) {
          iss >> t >> std::ws;
          DT_THROW_IF (! iss && !iss.eof(),
                       logic_error,
                       "'t' format error at invalid line '" << line << "' !");
          _next_time_ = t;
        }
        break;
      }
      if (_source_.eof ()) {
        break;
      }
    }
    return;
  }

  bool from_file_vg::_has_next ()
  {
    return geomtools::is_valid (_next_);
  }

  bool from_file_vg::has_next_vertex() const
  {
    return const_cast<from_file_vg*>(this)->_has_next ();
  }

  void from_file_vg::_shoot_vertex(::mygsl::rng & /*random_*/,
                                   ::geomtools::vector_3d & vertex_)
  {
    DT_THROW_IF (! is_initialized (), logic_error, "Not initialized !");
    // here apply the length unit:
    vertex_ = _next_ * _length_unit_;
    _read_next();
    return;
  }

  void from_file_vg::_shoot_vertex_and_time(::mygsl::rng & /*random_*/,
                                            ::geomtools::vector_3d & vertex_,
                                            double & time_)
  {
    DT_THROW_IF (! is_initialized (), logic_error, "Not initialized !");
    // Here apply the length/time units:
    vertex_ = _next_ * _length_unit_;
    time_ = _next_time_ * _time_unit_;
    _read_next();
    return;
  }

} // end of namespace genvtx
