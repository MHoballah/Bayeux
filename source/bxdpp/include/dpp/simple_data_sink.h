/* simple_data_sink.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-02-16
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Generic data sink
 *
 * History:
 *
 */

#ifndef DPP_SIMPLE_DATA_SINK_H_
#define DPP_SIMPLE_DATA_SINK_H_ 1

#include <string>

#include <datatools/things.h>

#include <dpp/dpp_config.h>
#include <dpp/i_data_sink.h>

// forward declaration:
namespace datatools {
  class data_writer;
}

namespace dpp {

  /// \brief A Boost/Serialization-based I/O data sink/writer
  class simple_data_sink : public i_data_sink
  {

  public:

    virtual void open ();

    virtual bool store_next_record (const datatools::things & a_event_record);

    virtual bool can_store_meta_data () const;

    virtual bool store_metadata (const datatools::properties & a_meta_data);

    virtual void close ();

    virtual void reset ();

    /// Constructor
    simple_data_sink (datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);


    /// Constructor
    simple_data_sink (const std::string & a_sink_label = "",
                      datatools::logger::priority a_priority = datatools::logger::PRIO_NOTICE);

    /// Destructor
    virtual ~simple_data_sink ();

  protected:

    void open_file_sink_ ();

    void close_file_sink_ ();

  private:

    unsigned int _record_counter_;
    unsigned int _metadata_counter_;
    datatools::data_writer * _boost_io_file_writer_;

  };

}  // end of namespace snemo

#endif // DPP_SIMPLE_SIMPLE_DATA_SINK_H_

// end of simple_data_sink.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
