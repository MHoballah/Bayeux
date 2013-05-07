// -*- mode: c++; -*-
/* wdecay0.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-04
 * Last modified: 2013-05-05
 *
 * License:
 * Copyright 2013 F. Mauger
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
 *   C++ wrapper GENBB/Decay0 C++ port generator.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_WDECAY0_H_
#define GENBB_HELP_WDECAY0_H_ 1

#include <iostream>
#include <boost/scoped_ptr.hpp>

#include <datatools/properties.h>
#include <mygsl/rng.h>

#include <genbb_help/i_genbb.h>

namespace genbb {

  namespace decay0 {
    struct bbpars;
  }

  class wdecay0 : public i_genbb
  {
  public:

    enum decay_type
      {
        DECAY_TYPE_UNDEFINED  = 0,
        DECAY_TYPE_DBD        = 1,
        DECAY_TYPE_BACKGROUND = 2
      };

    static const int DBD_MODE_INVALID = -1;

  public:

    bool is_debug () const;

    void set_debug (bool d_);

    virtual bool can_external_random () const;

    const mygsl::rng & get_random () const;

    mygsl::rng & grab_random ();

    size_t get_event_count () const;

    double get_to_all_events () const;

    wdecay0 ();

    virtual ~wdecay0 ();

    void dump (std::ostream & = std::clog) const;

    virtual void initialize (const datatools::properties & setup_,
                             datatools::service_manager & service_manager_,
                             detail::pg_dict_type & dictionary_);

    virtual void reset ();

    virtual bool has_next ();

    virtual bool is_initialized () const;

    genbb::decay0::bbpars & bb_params();

  protected:

    virtual void _load_next (primary_event & event_,
                             bool compute_classification_ = true);

  private:

    void _init_ ();

    void _reset_ ();

    void _set_decay_isotope_ (const std::string & di_);

  private:

    bool   _debug_;
    bool   _initialized_;

    int    _decay_type_;
    std::string _decay_isotope_;
    int    _decay_dbd_level_;
    int    _decay_dbd_mode_;
    size_t _event_count_;

    double _energy_min_;
    double _energy_max_;

    unsigned long _seed_;   /// PRNG seed (local or global)
    mygsl::rng    _random_; /// Local PRNG

    boost::scoped_ptr<genbb::decay0::bbpars> _bb_params_;

    GENBB_PG_REGISTRATION_INTERFACE(wdecay0);

  };

} // end of namespace genbb

#endif // GENBB_HELP_WDECAY0_H_

// end of wdecay0.h
