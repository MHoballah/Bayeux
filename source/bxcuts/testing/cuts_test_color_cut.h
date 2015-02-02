// -*- mode: c++ ; -*-
/* \file cuts_test_color_cut_h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-20
 * Last modified: 2010-09-20
 *
 * License:
 */

/**
 * Description:
 * A simple test data structure to perform
 *  simple cuts.
 *
 */

#ifndef __cuts__test__color_cut_h
#define __cuts__test__color_cut_h 1

#include <iostream>

#include <cuts/i_cut.h>

#include <cuts_test_data.h>

namespace cuts {

  namespace test {

    using namespace std;

    class color_cut : public cuts::i_cut
    {

    public:

      void set_color (int a_color);

    public:
      color_cut(datatools::logger::priority a_logging_priority =
            datatools::logger::PRIO_FATAL);
      virtual ~color_cut();

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

    private:

      int _color_;

      // Macro to automate the registration of the cut :
      CUT_REGISTRATION_INTERFACE(color_cut);

    };

  } // end of namespace test

} // end of namespace cuts

#endif // __cuts__test__color_cut_h
