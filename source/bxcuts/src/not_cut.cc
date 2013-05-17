// -*- mode: c++ ; -*-
/* not_cut.cc
 */

#include <cuts/not_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(not_cut, "cuts::not_cut");

  void not_cut::set_cut (cut_handle_type & a_cut_handle)
  {
    DT_THROW_IF(&a_cut_handle.grab () == this,
                std::logic_error,
                "Self-referenced NOT cut '"
                << get_name () << "' is not allowed !");
    _handle = a_cut_handle;
    return;
  }

  void not_cut::_at_set_user_data()
  {
    _export_user_data_to(_handle.grab());
  }

  void not_cut::_at_reset_user_data()
  {
    _handle.grab().reset_user_data();
  }

  CUT_CONSTRUCTOR_IMPLEMENT_HEAD(not_cut,a_logging_priority)
  {
    return;
  }

  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(not_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(not_cut)
  {
    DT_THROW_IF(! _handle,
                std::logic_error,
                "NOR cut '" << get_name() << "' has an invalid cut handle ! ");
    int status = _handle.grab().process();
    if (status == SELECTION_REJECTED) {
      return SELECTION_ACCEPTED;
    } else if (status == SELECTION_ACCEPTED) {
      return SELECTION_REJECTED;
    }
    return status;
  }


  CUT_RESET_IMPLEMENT_HEAD (not_cut)
  {
    _set_initialized (false);
    _handle.reset ();
    this->i_cut::_reset ();
    return;
  }


  CUT_INITIALIZE_IMPLEMENT_HEAD(not_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Cut '" << get_name () << "' is already initialized !");
    _common_initialize(a_configuration);
    if (! _handle) {
      std::string cut_name;
      DT_THROW_IF(!a_configuration.has_key("cut"),
                  std::logic_error,
                  "Missing 'cut' name property for NOT cut !");
      cut_name = a_configuration.fetch_string("cut");
      cut_handle_dict_type::iterator found = a_cut_dict.find(cut_name);
      DT_THROW_IF(found == a_cut_dict.end(),
                  std::logic_error,
                  "Can't find any cut named '"
                  << cut_name
                  << "' from the external dictionnary for this NOT cut ! ");
      this->set_cut(found->second.grab_initialized_cut_handle());
    }
    _set_initialized (true);
    return;
  }

} // end of namespace cuts

// end of not_cut.cc
