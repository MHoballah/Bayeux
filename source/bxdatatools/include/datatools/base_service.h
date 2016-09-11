/// \file datatools/base_service.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2016-02-18
 *
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 * Description:
 *
 *   Base service.
 *
 * History:
 *
 */
#ifndef DATATOOLS_BASE_SERVICE_H
#define DATATOOLS_BASE_SERVICE_H

// Standard Library
#include <iostream>
#include <map>
#include <stdexcept>
#include <typeinfo>
#include <vector>

// Third Party
#include <boost/function/function_base.hpp>

// This project (Bayeux/datatools)
#include <datatools/enriched_base.h>
#include <datatools/i_tree_dump.h>
#include <datatools/service_tools.h>
#include <datatools/factory_macros.h>
#include <datatools/exception.h>
#include "datatools/factory-inl.h"

#include <datatools/logger.h>
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

/// \brief Top-level namespace of the Bayeux/datatools module library
namespace datatools {

  // Forward class declaration:
  class properties;
  class object_configuration_description;

  /*! \brief The base service class
   *
   *  The base_service class provides the interface of all
   *  datatools service objects.
   */
  class base_service : public datatools::enriched_base {
  public:

    /// Default constructor
    base_service();

    /// Constructor
    base_service(const std::string& name,
                 const std::string& display_name = "",
                 const std::string& description = "",
                 logger::priority lp = logger::PRIO_FATAL);

    /// Destructor
    virtual ~base_service();

    /// Undocumented unused/unimplemented method
    virtual void fetch_dependencies(service_dependency_dict_type& /*dependencies*/) const;

    /// Initialize the service without a list of properties nor other services
    virtual int initialize_simple();

    /// Initialize the service using only a list of properties without the needs of other services
    virtual int initialize_standalone(const datatools::properties& config);

    /// Initialize the service using with access to a dictionary of other services
    virtual int initialize_with_services(service_dict_type& service_dict);

    /// Initialize the service using a list of properties with access to a dictionary of other services
    virtual int initialize(const datatools::properties& config,
                           service_dict_type& service_dict) = 0;

    /// Reset the service
    virtual int reset() = 0;

    /// Check if service is initialized
    virtual bool is_initialized() const = 0;

    /// Return the service class
    virtual std::string service_class_id() const = 0;

    /// Smart print
    virtual void tree_dump(std::ostream& out = std::clog,
                           const std::string & title = "",
                           const std::string & indent = "",
                           bool inherit = false) const;

    /// Basic OCD support shared by all inherited modules
    static void common_ocd(datatools::object_configuration_description & ocd_);

  protected:

    /// Common initialization of services
    void common_initialize(const datatools::properties & config);

     // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_service)

#ifndef Q_MOC_RUN
      //! Reflection interface
      DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };

  //! Find the service name with given service identifier from a dictionary of services
  bool find_service_name_with_id(const service_dict_type & services_,
                                 const std::string & service_id_,
                                 std::string & service_name_);

  //! Find all service names with given service identifier from a dictionary of services
  bool find_service_names_with_id(const service_dict_type & services_,
                                  const std::string & service_id_,
                                  std::vector<std::string> & service_names_);

  //! Return the mutable reference to a service given its name
  base_service & grab_service(service_dict_type & services_,
                              const std::string & service_name_);

  //! Return the const reference to a service given its name
  const base_service & get_service(const service_dict_type & services_,
                                   const std::string & service_name_);

  /** Return a mutable reference to a typed service object with given name from a dictionary of services
   *   @param services_ The dictionary of service entries
   *   @param service_name_ The name of the service to be checked
   *   @return a mutable reference to the service instance requested by name and type
   */
  template<class T>
  T& grab(service_dict_type & services_, const std::string& service_name_)
  {
    base_service & srvc = grab_service(services_, service_name_);
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(srvc);
    DT_THROW_IF(ti != tf, std::logic_error, "Service '" << service_name_ << "' is not of requested type!");
    return dynamic_cast<T&>(srvc);
  }

  /** Return a non mutable reference to a typed service object with given name from a dictionary of services
   *   @param services_ The dictionary of service entries
   *   @param service_name_ The name of the service to be checked
   *   @return a const reference to the service instance requested by name and type
   */
  template<class T>
  const T& get(const service_dict_type & services_, const std::string& service_name_)
  {
    const base_service & srvc = get_service(services_, service_name_);
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(srvc);
    DT_THROW_IF(ti != tf, std::logic_error, "Service '" << service_name_ << "' is not of requested type!");
    return dynamic_cast<const T&>(srvc);
  }

}  // end of namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_CLASS_INIT(datatools::base_service)
#endif // Q_MOC_RUN

#define DATATOOLS_SERVICE_REGISTRATION_INTERFACE(SERVICE_CLASS_NAME)    \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::datatools::base_service, SERVICE_CLASS_NAME) \
  public:                                                                 \
  virtual std::string service_class_id() const;                           \
  /**/

#define DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(SERVICE_CLASS_NAME,SERVICE_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::datatools::base_service,SERVICE_CLASS_NAME,SERVICE_ID) \
  std::string SERVICE_CLASS_NAME::service_class_id() const {return SERVICE_ID;}
  /**/

#endif // DATATOOLS_BASE_SERVICE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
