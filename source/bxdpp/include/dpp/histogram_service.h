/* histogram_service.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-10-23
 * Last modified : 2013-02-16
 *
 * Copyright (C) 2012-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   Histogram service.
 *
 * History:
 *
 */

#ifndef DPP_HISTOGRAM_SERVICE_H_
#define DPP_HISTOGRAM_SERVICE_H_ 1

#include <string>
#include <vector>

#include <dpp/dpp_config.h>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/services/base_service.h>
#include <datatools/services/service_tools.h>
#include <datatools/services/service_macros.h>
#else
#include <datatools/base_service.h>
#include <datatools/service_tools.h>
#include <datatools/service_macros.h>
#endif

#include <mygsl/histogram_pool.h>

class TH1D;
class TH2D;

namespace datatools {
#if DPP_DATATOOLS_LEGACY == 1
  namespace utils {
#endif
    class properties;
#if DPP_DATATOOLS_LEGACY == 1
  }
#endif
}

namespace dpp {

  /// \brief A service for managing 1D and 2D histograms
  class histogram_service : public DPP_DS::base_service
  {
  public:

    bool has_output_files () const;

    const std::vector<std::string> & get_output_files () const;

    std::vector<std::string> & grab_output_files ();

    void add_output_file (const std::string & fn_);
 
    const mygsl::histogram_pool & get_pool () const;

    mygsl::histogram_pool & grab_pool ();

    histogram_service ();

    virtual ~histogram_service ();

    virtual bool is_initialized () const;

    virtual int initialize (const DPP_DU::properties    & config_,
                            DPP_DS::service_dict_type & service_dict_);

    virtual int reset ();

    void store_as_boost_file (const std::string & filename_) const;

    void store_as_root_file (const std::string & filename_) const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    static void export_to_root (const mygsl::histogram_1d & h1d_,
                                const std::string & name_,
                                const std::string & title_,
                                TH1D & rh1d_);

    static void export_to_root (const mygsl::histogram_2d & h2d_,
                                const std::string & name_,
                                const std::string & title_,
                                TH2D & rh2d_);

  protected:

    void _at_init ();

    void _at_reset ();

  private:

    bool                      _initialized_;
    mygsl::histogram_pool     _pool_;
    std::vector<std::string>  _output_files_;

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE (histogram_service);

  };

}  // end of namespace dpp

#endif // DPP_HISTOGRAM_SERVICE_H_

// end of histogram_service.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
