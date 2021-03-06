// i_genbb.cc
/*
 * Copyright 2007-2018 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 *
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <genbb_help/i_genbb.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// This project:
#include <genbb_help/primary_event.h>

namespace genbb {

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_genbb,"genbb::i_genbb/__system__")

  const std::string & i_genbb::get_name() const
  {
    return _name_;
  }

  bool i_genbb::has_name() const
  {
    return ! _name_.empty();
  }

  void i_genbb::set_name(const std::string & name_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Event generator '" << get_name() << "' is initialized/locked !");
    _name_ = name_;
  }

  void i_genbb::reset_name()
  {
    _name_.clear();
  }

  bool i_genbb::is_debug() const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  datatools::logger::priority i_genbb::get_logging_priority() const
  {
    return _logging_priority;
  }

  void i_genbb::set_logging_priority(datatools::logger::priority p)
  {
    _logging_priority = p;
  }

  i_genbb::i_genbb()
  {
    _logging_priority = datatools::logger::PRIO_FATAL;
    _external_random_ = nullptr;
    _assign_generation_ids_ = false;
    return;
  }

  i_genbb::~i_genbb()
  {
    return;
  }

  void i_genbb::load_next(primary_event & event_,
                          bool compute_classification_)
  {
    _load_next(event_, compute_classification_);
    if (_assign_generation_ids_) {
      event_.assign_generation_ids();
    }
    return;
  }

  bool i_genbb::has_external_random() const
  {
    return _external_random_ != nullptr;
  }

  bool i_genbb::can_external_random() const
  {
    return false;
  }

  void i_genbb::reset_external_random()
  {
    _external_random_ = nullptr;
    return;
  }

  void i_genbb::set_external_random(mygsl::rng & r_)
  {
    DT_THROW_IF(! can_external_random(), std::logic_error, "This event generator does not accept external PRNG !");
    DT_THROW_IF(! r_.is_initialized(), std::logic_error, "External PRNG is not initialized !");
    _external_random_ = &r_;
    return;
  }

  mygsl::rng & i_genbb::grab_external_random()
  {
    DT_THROW_IF(! has_external_random(), std::logic_error, "No available external PRNG !");
    return *_external_random_;
  }

  const mygsl::rng & i_genbb::get_external_random() const
  {
    DT_THROW_IF(! has_external_random(), std::logic_error, "No available external PRNG !");
    return *_external_random_;
  }

  void i_genbb::initialize_simple()
  {
    datatools::properties dummy_setup;
    initialize_standalone(dummy_setup);
    return;
  }

  void i_genbb::initialize_standalone(const datatools::properties & setup_)
  {
    datatools::service_manager dummy_srvcmgr;
    detail::pg_dict_type dummy_dict;
    initialize(setup_, dummy_srvcmgr, dummy_dict);
    return;
  }

  void i_genbb::initialize_with_dictionary_only(const datatools::properties & setup_,
                                                detail::pg_dict_type & dictionary_)
  {
    datatools::service_manager dummy_srvcmgr;
    initialize(setup_, dummy_srvcmgr, dictionary_);
    return;
  }

  void i_genbb::initialize_with_service_only(const datatools::properties & setup_,
                                             datatools::service_manager & service_manager_)
  {
    detail::pg_dict_type dummy_dict;
    initialize(setup_, service_manager_, dummy_dict);
    return;
  }

  void i_genbb::_initialize_base(const datatools::properties & setup_)
  {
    // Logging priority :
    datatools::logger::priority p
      = datatools::logger::extract_logging_configuration(setup_,
                                                         datatools::logger::PRIO_FATAL);
    DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                std::domain_error,
                "Unknown logging priority configuration rule !");
    set_logging_priority(p);

    // Name of the generator (only if not already set) :
    if (_name_.empty()) {
      if (setup_.has_key("name")) {
        set_name(setup_.fetch_string("name"));
      }
    }

    if (setup_.has_key("assign_generation_ids")) {
      bool value = setup_.fetch_boolean("assign_generation_ids");
      set_assign_generation_ids(value);
    }

    return;
  }

  void i_genbb::set_assign_generation_ids(bool value_)
  {
    _assign_generation_ids_ = value_;
    return;
  }

  bool i_genbb::is_assign_generation_ids() const
  {
    return _assign_generation_ids_;
  }

  void i_genbb::tree_dump(std::ostream & a_out,
                          const std::string & a_title,
                          const std::string & a_indent,
                          bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) indent = a_indent;
    if (! a_title.empty()) a_out << indent << a_title << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Name                  : '" << _name_ << "'" << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Logging priority      : '"
          << datatools::logger::get_priority_label(_logging_priority) << "'"
          << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Assign generation Ids : '" << _assign_generation_ids_ << "'" << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Can use external PRNG : " << can_external_random()  << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Has external PRNG     : " << has_external_random()  << std::endl;

    return;
  }

  // static
  void i_genbb::base_initialize_ocd(datatools::object_configuration_description & ocd_)
  {
    datatools::logger::declare_ocd_logging_configuration(ocd_,
                                                         "fatal",
                                                         "",
                                                         "genbb::i_genbb");

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("assign_generation_ids")
        .set_from("genvtx::i_genbb")
        .set_terse_description("Set the flag to assign generation IDs to primary particles")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .set_long_description("The flag to assign generation ids to primary particles \n"
                              "Particles are given unique Ids from 0 to N-1 where N   \n"
                              "is the total number of primary particles in the event. \n"
                              )
        .add_example("Activate the assignment of unique generation ids:    \n"
                     "                                                     \n"
                     "  assign_generation_ids : boolean = true             \n"
                     "                                                     \n"
                     )
        ;
    }

    return;
  }

} // end of namespace genbb
