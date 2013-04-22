// -*- mode: c++ ; -*-
/* manager.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-22
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   A manager for isotopes/elements and materials
 *
 * History:
 *
 */

#ifndef MATERIALS_MANAGER_H_
#define MATERIALS_MANAGER_H_ 1

#include <ostream>
#include <string>
#include <list>

#include <materials/factory.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>

namespace datatools {
  class multi_properties;
}

namespace materials {

  class manager : public datatools::i_tree_dumpable
  {

  public:

    bool is_initialized () const;

    bool is_debug () const;

    void set_debug (bool);

    bool is_locked () const;

    bool is_alias_allow_overload () const;

    void set_alias_allow_overload (bool aao_);

    bool has_material(const std::string & entry_name_) const;

    bool is_alias(const std::string & entry_name_) const;

    std::string alias_of(const std::string & entry_name_) const;

    const isotope_dict_type & get_isotopes () const;

    const element_dict_type & get_elements () const;

    const material_dict_type & get_materials () const;

    const std::list<std::string> & get_ordered_materials () const;

    /// Constructor
    manager ();

    /// Destructor
    virtual ~manager ();

    void load (const datatools::multi_properties & config_);

    void initialize (const datatools::properties & setup_);

    void reset ();

    void lock ();

    void unlock ();

    //! Print info
    virtual void tree_dump (std::ostream & out_    = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  private:

    bool                   _debug_; /// Debug flag
    bool                   _locked_; /// Lock flag
    bool                   _alias_allow_overload_; /// Flag to allow alias overloading
    factory                _creator_; /// Embeded factory
    isotope_dict_type      _isotopes_; /// Dictionnary of isotopes
    element_dict_type      _elements_; /// Dictionnary of elements
    material_dict_type     _materials_; /// Dictionnary of materials
    std::list<std::string> _ordered_materials_; /// Ordered list of materials by name

  };

} // end of namespace materials

//namespace mat = materials;

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(materials::manager)

#endif // MATERIALS_MANAGER_H_

// end of manager.h
