/// \file cuts/not_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   NOT logic unary cut.
 *
 * History:
 *
 */

#ifndef CUTS_NOT_CUT_H
#define CUTS_NOT_CUT_H 1

// This project:
#include <cuts/i_cut.h>

namespace cuts {

  /// \brief The NOT cut
  class not_cut : public cuts::i_cut
  {
  public:

    void set_cut(cut_handle_type &);

    /// Constructor
    not_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~not_cut();

    /// Initialization
    virtual void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &);

    /// Export to a container of properties
    virtual void export_to_config(datatools::properties & config_,
                                  uint32_t flags_ = i_cut::EXPORT_CONFIG_DEFAULT,
                                  const std::string & prefix_ = "") const;

    /// Reset
    virtual void reset();

  protected :

    /// Selection
    virtual int _accept();

  protected:

    virtual void _at_set_user_data();

    virtual void _at_reset_user_data();

  protected:

    cut_handle_type _handle; //!< The cut to be negated by the "NOT" cut class

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(not_cut)

  };

} // end of namespace cuts

#endif // _CUTS_NOT_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
