// -*- mode: c++ ; -*- 
/* reader.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-11-01
 * Last modified: 2010-11-01
 * 
 * License: 
 * 
 * Description: 
 *
 *   A Boost archive from ROOT file reader
 * 
 * History: 
 * 
 */

#ifndef __brio__reader_h
#define __brio__reader_h 1

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/archive/text_iarchive.hpp>
// future:
#include <boost/archive/portable_binary_iarchive.hpp>

#include <brio/detail/base_io.h>

#include <TTree.h>

namespace brio {

  using namespace std;

  class reader : public base_io
  {    
  private: 

    bool _allow_mixed_types_in_stores_;
    bool _allow_automatic_store_;
    bool _check_serial_tag_;
    store_info * _automatic_store_;

  public:

    // setters:

    /** By default, serialization tag are checked each time
     *  an object is deserialized.
     *  This behaviour can be changed:
     *
     *  brio::reader a_reader;
     *  a_reader.open ("file.root");
     *  a_reader.set_check_serial_tag (false);
     *
     */
    void set_check_serial_tag (bool new_value_ = true);

    bool is_check_serial_tag () const;

    /** Position current entry of store 'label' just before the first
     *  serialized object
     */
    void rewind_store (const string & label_ = "");

    /** Position current entry of store 'label' just after the last
     *  serialized object
     */
    void unwind_store (const string & label_ = "");

    bool has_previous (const string & label_ = "") const;

    bool has_next (const string & label_ = "") const;

  protected:

    void _set_default ();

  public: 

    // ctor:
    reader ();

    // ctor:
    reader (const string & filename_, 
	    bool verbose_ = false, 
	    bool debug_ = false);

    // ctor:
    reader (const string & filename_, 
	    const string & format_str_,
	    bool verbose_ = false, 
	    bool debug_ = false);

    // dtor:
    virtual ~reader ();

    // Utilities:
    virtual void tree_dump (std::ostream & out_ = std::clog, 
			    const std::string & title_ = "", 
			    const std::string & indent_ = "", 
			    bool inherit_ = false) const;

    void print_info (ostream & out_ = clog) const;

    // Main load template methods:
    template<class T>
    int load_next (T & data_, 
		   const string & label_ = "")
    {
      int64_t entry = get_current_entry (label_);
      return load<T> (data_, label_, entry + 1);
    }

    template<class T>
    int load_previous (T & data_, 
		       const string & label_ = "")
    {
      int64_t entry = get_current_entry (label_);
      return load<T> (data_, label_, entry - 1);
    }

    template<class T>
    int load (T & data_, int64_t nentry_ = -1)
    {
      return load <T> (data_, "", nentry_);
    }
    
    template<class T>
    int load (T & data_, const string & label_, int64_t nentry_ = -1)
    {
      _only_if_opened ("brio::reader::load");
      store_info * ptr_si = _get_store_info (label_);
      if (ptr_si == 0)
	{
	  if (label_.empty ())
	    {
	      // if we do not allow automatic store, this is a critical error:
	      if (! _allow_automatic_store_)
		{
		  ostringstream message;
		  message << "brio::reader::load: "
			  << "No source store is selected nor default store is available !";
		  throw logic_error (message.str ());
		}
	      else
		{
		  ptr_si = _automatic_store_;
		}
	    }
	  else
	    {
	      ostringstream message;
	      message << "brio::reader::load: "
		      << "No source store with label '" << label_ << "' !";
	      throw logic_error (message.str ());
	    }
	}
      return _at_load<T> (data_, ptr_si, nentry_);
    }

  protected:

    virtual void _at_open (const string & filename_);

    template<class T>
    int _at_load (T & data_, 
		  store_info * ptr_si_, 
		  int64_t nentry_)
    {
      if (is_debug ())
	{
	  cerr << "DEBUG: " << "brio::reader::_at_load: "
	       << "Entering..." << endl;
	}
      store_info & si = *ptr_si_;

      if (_check_serial_tag_)
	{
	  // We check if the serialization tag from the store matches the data's one:
	  if (si.has_dedicated_serialization_tag () 
	      && (data_.get_serial_tag () != si.get_serialization_tag ()))
	    {
	      ostringstream message;
	      message << "brio::reader::_at_load: "
		      << "Data serialization tag '" << data_.get_serial_tag () 
		      << "' does not match source store's serialization tag '" << si.get_serialization_tag () << "' !";
	      throw logic_error (message.str ());
	    }
	}
      
      if (si.number_of_entries == 0)
	{
	  ostringstream message;
	  message << "brio::reader::_at_load: "
		  << "Source store '" << si.label << "' has no entry !";
	  throw logic_error (message.str ());
	}
      
      int64_t nentry = nentry_;
      if (nentry >= 0)
	{
	  // check overflow:
	  if (nentry_ >= si.number_of_entries)
	    {
	      cerr << "DEVEL: brio::reader::_at_load: nentry=" << nentry_ << endl;
	      cerr << "DEVEL: brio::reader::_at_load: si.number_of_entries=" << si.number_of_entries << endl;
	      ostringstream message;
	      message << "brio::reader::_at_load: "
		      << "Source store '" << si.label << "' has "
		      << "no serialized entry at index '" << nentry_ << "' !";
	      throw logic_error (message.str ());
	    }
	}
      else
	{
	  // if nentry_ < 0: use entry index relative to the current entry position
	  if (si.current_entry < 0) // at rewind position
	    {
	      // start with first entry:
	      nentry = 0;
	    }
	  else
	    {
	      // try next entry:
	      nentry = si.current_entry + 1;
	    }
	}

      // read this tree entry in the ROOT I/O system:
      si.record.reset ();
      int ret = si.tree->GetEntry (nentry, 1); // -> 1 for all branches
      if (ret == 0)
	{
	  ostringstream message;
	  message << "brio::reader::_at_load: "
		  << "No entry '" 
		  << nentry << "' at entry # " << nentry 
		  << " in source store labelled '" << si.label.c_str () 
		  << "' from  file '" << _filename << "' !";
	  throw logic_error (message.str ());  	    	    
	}
      else if (ret == -1)
	{
	  ostringstream message;
	  message << "brio::reader::_at_load: "
		  << "An I/O error occurs from entry '" 
		  << nentry << "' in source store labelled '" << si.label.c_str () 
		  << "' from  file '" << _filename << "' !";
	  throw runtime_error (message.str ());  	    	    
	}
      else
	{
	  si.current_entry = nentry;
	}
      
      if (_check_serial_tag_)
	{
	  /* We may be confused with stores without dedicated serialization tag.
	   * Here we test if data and the entry's serial tags match:
	   */
	  if (! si.has_dedicated_serialization_tag ())
	    {
	      // check serial tag associated to the buffered binary archive:
	      string serial_tag = si.record.fSerialTag.Data ();
	      if (data_.get_serial_tag () != serial_tag)
		{
		  ostringstream message;
		  message << "brio::reader::_at_load: "
			  << "Entry '" 
			  << nentry << "' with serial tag '" << serial_tag << "' in (mixed) source store labelled '" << si.label.c_str () 
			  << "' from  file '" << _filename << "' does not match the requested '" 
			  << data_.get_serial_tag () << "' data type !";
		  throw logic_error (message.str ());  	    	    
		}
	    }
	}
      
      // Deserialize from the archive:
      boost::iostreams::stream<boost::iostreams::array_source> input_stream (si.record.fDataBuffer.fArray, 
									     si.record.fDataBuffer.fN);
      // 2011-06-16 FM: restored 
      if (is_format_pba ())
	{
	  boost::archive::portable_binary_iarchive ia (input_stream);   
	  ia >> data_;
	}
      if (is_format_text ())
	{
	  input_stream.imbue (*_locale);
	  boost::archive::text_iarchive ia (input_stream);   
	  ia >> data_;
	}
      
      _current_store = &si;
      if (is_debug ())
	{
	  cerr << "DEBUG: " << "brio::reader::_at_load: "
	       << "Exiting." << endl;
	}
      return 0;
    }
    
  };

} // end of namespace brio

#endif // __brio__reader_h

// end of reader.h
