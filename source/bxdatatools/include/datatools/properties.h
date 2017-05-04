/// \file datatools/properties.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-19
 * Last modified: 2017-03-13
 *
 * License:
 *
 * Copyright (C) 2008-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A simple properties dictionary with I/O functionalities.
 *
 */
#ifndef DATATOOLS_PROPERTIES_H
#define DATATOOLS_PROPERTIES_H

// Standard Library:
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// This Project:
#include <datatools/datatools_config.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/i_cloneable.h>
#include <datatools/bit_mask.h>
#include <datatools/exception.h>

namespace datatools {

  //! \brief A dictionary of arbitrary properties.
  /**
   *  The properties class provides a serializable container that holds
   *  many different data of different types (bool, int, double, string)
   *  as well as fixed size arrays (std::vector) of these build-in types.
   *
   */
  class properties :
    public datatools::i_serializable,
    public datatools::i_tree_dumpable,
    public datatools::i_clear,
    public datatools::i_cloneable {
  public:
    //----------------------------------------------------------------------
    //
    //! \brief Internal data stored within the dictionary of the properties class.
    class data {
    public:
      static const int ERROR_SUCCESS = 0;
      static const int ERROR_FAILURE = 1;
      static const int ERROR_BADTYPE = 2;
      static const int ERROR_RANGE   = 3;
      static const int ERROR_LOCK    = 4;

      static const uint8_t MASK_TYPE          = 0x7;  // = 00000111
      static const uint8_t MASK_UNIT_SYMBOL   = 0x8;  // = 00001000 for real parameters
      static const uint8_t MASK_EXPLICIT_PATH = 0x10; // = 00010000 for string parameters
      static const uint8_t MASK_EXPLICIT_UNIT = 0x20; // = 00100000 for real parameters
      static const uint8_t MASK_LOCK          = 0x40; // = 01000000
      static const uint8_t MASK_VECTOR        = 0x80; // = 10000000

      static const uint8_t TYPE_NONE    = 0x0; // = 000
      static const uint8_t TYPE_BOOLEAN = 0x1; // = 001
      static const uint8_t TYPE_INTEGER = 0x2; // = 010
      static const uint8_t TYPE_REAL    = 0x3; // = 011
      static const uint8_t TYPE_STRING  = 0x4; // = 100

      static const char TYPE_BOOLEAN_SYMBOL = 'B';
      static const char TYPE_INTEGER_SYMBOL = 'I';
      static const char TYPE_REAL_SYMBOL    = 'R';
      static const char TYPE_STRING_SYMBOL  = 'S';

      static const char STRING_FORBIDDEN_CHAR = '"';

      static const int SCALAR_DEF  = -1;
      static const int SCALAR_SIZE =  1;

      /// \brief Provides static method for default values for each supported type
      struct defaults {
        static bool boolean_value(); // return false
        static int integer_value();  // return 0
        static double real_value();  // return 0.0
        static const std::string string_value(); // return empty string
      };

    public:
      typedef std::vector<bool>        vbool;   ///< Container for boolean data
      typedef std::vector<int32_t>     vint;    ///< Container for integer data
      typedef std::vector<double>      vdouble; ///< Container for real data
      typedef std::vector<std::string> vstring; ///< Container for character string data

    public:

      /// Constructor
      data(char a_type = TYPE_INTEGER_SYMBOL,
           int a_size = SCALAR_DEF);

      /// Constructor
      data(bool a_value, int a_size = SCALAR_DEF);

      /// Constructor
      data(int a_value, int a_size = SCALAR_DEF);

      /// Constructor
      data(double a_value, int a_size = SCALAR_DEF);

      /// Constructor
      data(const std::string& a_value, int a_size = SCALAR_DEF);

      /// Constructor
      data(const char* a_value, int a_size = SCALAR_DEF);

      /// Destructor
      virtual ~data();

      /// Check is description is set
      bool has_description() const;

      /// Set the description string associated to the stored data
      void set_description(const std::string&);

      /// Get the description string associated to the stored data
      const std::string& get_description() const;

      /// Set the unit symbol associated to the stored real data
      int set_unit_symbol(const std::string&);

      /// Get the unit symbol associated to the stored real data
      const std::string& get_unit_symbol() const;

      /// Check if the data type is valid
      bool has_type() const;

      /// Return type
      int get_type() const;

      /// Check if the data is a boolean value
      bool is_boolean() const;

      /// Check if the data is a integer value
      bool is_integer() const;

      /// Check if the data is a real value
      bool is_real() const;

      /// Check if the data is a string value
      bool is_string() const;

      /// Check if the data is a path string value
      bool is_path() const;

      /// Check if the data is scalar (exactly one value)
      bool is_scalar() const;

      /// Check if the data is vector (>=0 stored values in an array)
      bool is_vector() const;

      /// Check if the data is locked (cannot be modified)
      bool is_locked() const;

      /// Check if the data has an unit symbol
      bool has_unit_symbol() const;

      /// Check if the (real only) data has been initialized with explicit unit
      bool has_explicit_unit() const;

      /// Check if the (string only) data has been initialized with explicit path
      bool is_explicit_path() const;

      /// Check if the data is not locked (can be modified)
      bool is_unlocked() const;

      /// Assign N boolean values
      int boolean(int a_size = SCALAR_DEF);

      /// Assign N integer values
      int integer(int a_size = SCALAR_DEF);

      /// Assign N real values
      int real(int a_size = SCALAR_DEF);

      /// Assign N string values
      int string(int a_size = SCALAR_DEF);

      /// Lock the value (make it unmutable)
      int lock();

      /// Unlock the value (make it mutable)
      int unlock();

      /// Returns the size of the array of stored values (1 if scalar, >=0 if vector)
      int32_t get_size() const;

      /// Returns the size of the array of stored values (1 if scalar, >=0 if vector)
      int32_t size() const;

      /// Check if the value array is empty
      bool empty() const;

      /// Check if array index is valid
      bool index_is_valid(int) const;

      /// Get the boolean value stored at a given rank
      bool get_boolean_value(int = 0) const;

      /// Get the integer value stored at a given rank
      int get_integer_value(int = 0) const;

      /// Get the real value stored at a given rank
      double get_real_value(int = 0) const;

      /// Get the string value stored at a given rank
      std::string get_string_value(int = 0) const;

      /// Set the boolean value at a given rank
      int set_value(bool, int = 0);

      /// Set the integer value at a given rank
      int set_value(int, int = 0);

      /// Set the real value at a given rank
      int set_value(double, int = 0, bool a_explicit_unit = false);

      /// Set the real value at a given rank
      int set_value_with_unit(double, int = 0, const std::string & a_unit_symbol = "");

      /// Set the explicit unit flag
      int set_explicit_unit(bool);

      /// Set the explicit path flag
      int set_explicit_path(bool);

      /// Set the string value at a given rank
      int set_value(const std::string&, int = 0, bool a_explicit_path = false);

      /// Set the string value at a given rank
      int set_value(const char*, int = 0, bool a_explicit_path = false);

      /// Get the boolean value by reference stored at a given rank
      int get_value(bool&, int = 0) const;

      /// Get the integer value by reference stored at a given rank
      int get_value(int&, int = 0) const;

      /// Get the real value by reference stored at a given rank
      int get_value(double &, int = 0) const;

      /// Get the string value by reference stored at a given rank
      int get_value(std::string&, int = 0) const;

      /// Get a string label associated to the type of the stored data
      std::string get_type_label() const;

      /// Get a string label associated to the scalar/vector trait of the stored data
      std::string get_vector_label() const;

      /// Check if a string contains a forbidden character
      static bool has_forbidden_char(const std::string&);

      /// Basic print
      void dump(std::ostream&) const;

      /// Convert to string and print in an output stream
      void to_string(std::ostream& a_out) const;

      /// Returns an error message from an integer error code
      static std::string get_error_message(int);

      //! Method for smart printing (from the datatools::i_tree_dump interface).
      virtual void tree_dump(std::ostream& a_out = std::clog,
                             const std::string& a_title  = "",
                             const std::string& a_indent = "",
                             bool a_inherit = false) const;

    private:
      void clear_values_();

      void clear_unit_symbol_();

      int init_values_(char a_type = TYPE_INTEGER_SYMBOL,
                       int a_size = SCALAR_DEF);

      BOOST_SERIALIZATION_BASIC_DECLARATION()

    private:
      std::string _description_; //!< Description of the property
      /** 8-bits description flags :
       * Format is : VLUPSTTT
       *  V   == vector bit
       *  L   == lock bit
       *  U   == explicit unit bit (real)
       *  P   == path bit (string)
       *  S   == unit symbol (real)
       *  TTT == type bits
       */
      uint8_t     _flags_;          //!< Traits
      vbool       _boolean_values_; //!< Stored boolean values
      vint        _integer_values_; //!< Stored integer values
      vdouble     _real_values_;    //!< Stored real values
      vstring     _string_values_;  //!< Stored string values
      std::string _unit_symbol_;    //!< Preferred unit symbol for real properties

    }; //----- end of data inner class


    //----------------------------------------------------------------------
    // Key validator inner classes
    //! \brief Pure abstract class for key validator.
    struct basic_key_validator
      : public std::unary_function<std::string,bool> {
      virtual ~basic_key_validator(){}
      virtual bool operator()(const std::string& a_key_arg) const = 0;
    };

    //! \brief Default abstract class for key validator.
    class default_key_validator : public basic_key_validator {
    public:
      static const std::string & allowed_chars();

      /// Default constructor
      default_key_validator();

      /// Destructor
      virtual ~default_key_validator ();

      /// Object function interface
      virtual bool operator()(const std::string& a_key_arg) const;
    };

    //----------------------------------------------------------------------
    // Config inner class
    //
    friend class config;
    //! \brief Class for ASCII file I/O operations with properties objects.
    class config {
    public:

      static const std::string & lock_decorator();
      static const std::string & as_directive();
      static const std::string & in_directive();
      static const std::string & path_decorator();
      static const std::string & metacomment_prefix();

      /// \brief Decoration mode
      /// @deprecated is replaced by
      /// the dedicated options_flag (HEADER_FOOTER) in constructor.
      enum decoration_mode_type {
        MODE_BARE          = 0,         ///< No decoration
        MODE_HEADER_FOOTER = 1,         ///< Header/footer decoration
        MODE_DEFAULT       = MODE_BARE, ///< Default decoration mode
        mode_bare          = MODE_BARE,
        mode_header_footer = MODE_HEADER_FOOTER
      };

      /// \brief Option flags used at construction
      enum options_flag {
        SKIP_PRIVATE     = bit_mask::bit00, ///< Skip private properties bit
        FORBID_VARIANTS  = bit_mask::bit01, ///< Forbid variant directives bit
        LOG_MUTE         = bit_mask::bit02, ///< Mute mode activation bit
        LOG_DEBUG        = bit_mask::bit03, ///< Debug mode activation bit
        LOG_TRACE        = bit_mask::bit04, ///< Trace mode activation bit
        SMART_MODULO     = bit_mask::bit05, ///< Use smart modulo (write)
        HEADER_FOOTER    = bit_mask::bit06, ///< Use header/footer (write)
        REQUESTED_TOPIC  = bit_mask::bit07, ///< Requested topic (read/write)
        FORBID_INCLUDES  = bit_mask::bit08, ///< Forbid include directives bit
        DONT_CLEAR       = bit_mask::bit09, ///< Don't clear before parsing bit (read)
        RESOLVE_PATH     = bit_mask::bit10  ///< Resolve path for input filename (read/write)
      };

    public:

      /// Constructor
      config(uint32_t options_ = 0,
             const std::string & topic_ = "",
             const std::string & section_name_ = "",
             int section_start_line_number_ = -1);

      /// Destructor
      virtual ~config();

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Set the logging priority threshold
      void set_logging(datatools::logger::priority);

      // /// @deprecated Check the debug flag
      // bool is_debug() const;

      // /// @deprecated Set the debug flag
      // void set_debug(bool a_debug);

      /// Read a properties container from an input stream
      void read(std::istream& a_in, properties& a_prop);

      /// Read a properties container from an input file
      void read(const std::string & a_in, properties& a_prop);

      /// Write a properties container to an output stream
      void write(std::ostream& a_out, const properties& a_prop);

      /// Write a properties container to an output file
      void write(const std::string & a_filename, const properties& a_prop);

      /// Return the current value of the line counter
      int get_current_line_number() const;

      /// Set the filename and the line counter before parsing
      void set_reader_input(const std::string & filename, int line_count = -1);

      /// Check if topic is set
      bool has_topic() const;

      /// Set the topic that should be matched
      void set_topic(const std::string & topic);

      /// Return the topic
      const std::string & get_topic() const;

      /// Check if section info is set
      bool has_section_info() const;

      /// Set the section info
      void set_section_info(const std::string & section_name_,
                            int section_start_line_number);

      /// Reset the section info
      void reset_section_info();

      /// Return the section
      const std::string & get_section_name() const;

      /// Return the section start line
      int get_section_start_line_number() const;

      /// Reset
      void reset();

      /// Write metacomment
      void write_metacomment(std::ostream& a_out,
                             const std::string & tag,
                             const std::string & value = "",
                             const std::string & comment = "");

      /// Write a property data
      void write_data(std::ostream& a_out,
                      const std::string & a_data_key,
                      const properties::data& a_prop_data,
                      const std::string & a_unit_symbol = "",
                      const std::string & a_unit_label = "",
                      const std::string & a_comment = "");

    private:

      /// Set default values at construction
      void init_defaults_();

      /// Parsing implementation
      void read_(std::istream& a_in, properties& a_prop);

      /// Saving implementation
      void write_(std::ostream& a_out, const properties& a_prop);

    private:

      // Configuration:
      logger::priority _logging_; ///< Logging priority threshold (read/write)
      int    _mode_;              ///< Decoration mode (header/footer)
      bool   _dont_clear_;        ///< Do not clear the container at the beginning of parsing (read)
      bool   _use_smart_modulo_;  ///< Use multiline formatting for vector data (write)
      bool   _write_public_only_; ///< Only output public properties
      bool   _forbid_variants_;   ///< Flag to forbid variant directives (read)
      bool   _forbid_includes_;   ///< Flag to forbid include directives (read)
      bool   _requested_topic_;   ///< Flag to activate topic matching (read)
      bool   _resolve_path_;      ///< Explicitely resolve path for input/output filenames (read/write)
      std::string _topic_;             ///< Topic to be validated
      std::string _section_name_;      ///< Current section name (from hosting multi_properties, read)
      int _section_start_line_number_; ///< Current section start line number (from hosting multi_properties, read)

      // Working parsing data:
      size_t      _current_line_number_; ///< Current input line number
      std::string _current_filename_;    ///< Current filename

    }; //----- end of class config


    //----------------------------------------------------------------------
    // properties class declarations
    //
  public:

    //! Prefix string used for the naming of private properties
    //!
    //! A private property has a key starting with this prefix,
    //! namely '__' (double underscore).
    static const std::string & private_property_prefix();

    // Typedefs declarations:
  protected:
    typedef std::map<std::string, data> pmap;
  public:
    typedef std::vector<std::string>    keys_col_type;

  public:
    /// Default constructor with embedded default key validator
    properties();

    /// Constructor with explicit description and embedded default key validator
    explicit properties(const std::string& a_desc);

    /// Constructor with explicit description and explicit key validator
    properties(const std::string & a_desc, const basic_key_validator&);

    /// Constructor with explicit key validator
    explicit properties(const basic_key_validator&);

    /* with external key validator (deletion_on_destroy_==false)
     * with internal key validator (deletion_on_destroy_==true)
     *      validator is deleted in the destructor.
     */

    /// Constructor with explicit description and explicit external key validator
    properties(const std::string& a_desc, const basic_key_validator*,
               bool deletion_on_destroy_ = true);

    /// Constructor with explicit external key validator
    explicit properties(const basic_key_validator*,
               bool deletion_on_destroy_ = true);

    /// Destructor
    virtual ~properties();

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool);

    /// Returns the number of stored properties
    int32_t size() const;

    /// Check if the properties container is empty
    bool empty() const;

    /// Set the description string of the properties container
    ///
    /// The description is a single line of text possibly splitted
    /// in subparts separated by the semicolon (';') character.
    ///
    /// Examples:
    /// \code
    /// Head
    /// Head;Text;...;Text;...
    /// ;Text;...;Text;...
    /// \endcode
    /// The leading (optional) subpart corresponds to a short text
    /// which describes the container of properties.
    /// The other subparts are optional and possibly add some auxiliary informations
    /// about the container of properties.
    ///
    /// Examples:
    /// \code
    /// My beautiful set of configuration parameter; group=geometry ; price=100 credits ; company=ACME
    /// \endcode
    /// The leading (optional) subpart corresponds to a short text
    void set_description(const std::string&);

    /// Returns the description string of the properties container
    const std::string& get_description() const;

    /// Fetch the short description string associated to the container
    bool fetch_short_description(std::string &) const;

    /// Check if a short description string is associated to the container
    bool has_short_description() const;

    /// Get the short description string associated to the container
    std::string get_short_description() const;

    /// Check if some auxiliary descriptions are set
    bool has_auxiliary_descriptions() const;

    /// Fetch the auxiliary description strings associated to the container
    bool fetch_auxiliary_descriptions(std::vector<std::string> &) const;

    /// Unset the current key validator
    void unset_key_validator();

    /// Use the default key validator
    void set_default_key_validator();

    /// Set the current key validator
    void set_key_validator(const basic_key_validator&);

    /// Set the current key validator with ownership
    void set_key_validator(const basic_key_validator*,
                           bool a_deletion_on_destroy = true);

    //! Returns the list of keys stored in the map (read-only).
    std::vector<std::string> keys() const;

    //! Returns the ith key
    const std::string & key (int) const;

    //! Set the list of keys.
    void keys(std::vector<std::string>&) const;

    //! Access to a non-mutable reference to a property data object
    const data & get(const std::string& prop_key) const;

    //! Store data item with supplied key
    void store(const std::string& key, const data& value);

    //! builds the list of keys (by reference) stored in the map that start with prefix.
    void keys_not_starting_with(std::vector<std::string>&, const std::string& prefix) const;

    //! returns the list of keys stored in the map that start with prefix.
    std::vector<std::string> keys_not_starting_with(const std::string& prefix) const;

    //! builds the list of keys (by reference) stored in the map that start with prefix.
    void keys_starting_with(std::vector<std::string>&, const std::string& prefix) const;

    //! returns the list of keys stored in the map that start with prefix.
    std::vector<std::string> keys_starting_with(const std::string& prefix) const;

    //! builds the list of keys (by reference) stored in the map that end with suffix.
    void keys_not_ending_with(std::vector<std::string>&, const std::string& suffix) const;

    //! returns the list of keys stored in the map that end with suffix.
    std::vector<std::string> keys_not_ending_with(const std::string& suffix) const;

    //! builds the list of keys (by reference) stored in the map that end with suffix.
    void keys_ending_with(std::vector<std::string>&, const std::string& suffix) const;

    //! returns the list of keys stored in the map that end with suffix.
    std::vector<std::string> keys_ending_with(const std::string& suffix) const;

    //! Lock the properties dictionary.
    void lock(const std::string& prop_key);

    //! Unlock the properties dictionary.
    void unlock(const std::string& prop_key);

    //! Check if the instance is locked.
    bool is_locked(const std::string& prop_key) const;

    //! Return a static key.
    static std::string make_private_key(const std::string& prop_key);

    //! Check if a string matches a private 'prop_key'.
    static bool key_is_private(const std::string& prop_key);

    //! Check if a string matches a public 'prop_key'.
    static bool key_is_public(const std::string& prop_key);

    //! Check if data with name 'prop_key' is private
    bool is_private(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is public
    bool is_public(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is boolean
    bool is_boolean(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is integer
    bool is_integer(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is real
    bool is_real(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is string
    bool is_string(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is scalar
    bool is_scalar(const std::string& prop_key) const;

    //! Check if data with name 'prop_key' is vector
    bool is_vector(const std::string& prop_key) const;

    // 2012-11-14 FM : Should be deprecated
    int32_t size(const std::string& prop_key) const;

    //! Returns the size of the data stored with a given key/name
    int32_t key_size(const std::string& prop_key) const;

    //! Check if a property with given key/name exists
    bool has_key(const std::string& prop_key) const;

    //! Lock a property with given key/name
    void key_lock (const std::string& prop_key);

    //! Unlock a property with given key/name
    void key_unlock (const std::string& prop_key);

    //! Get the description string associated to a property with given key/name
    const std::string & get_key_description (const std::string& prop_key) const;

    //! Set the description string associated to a property with given key/name
    void set_key_description (const std::string& prop_key, const std::string &desc_);

    // 2011-11-27 FM: could be useful
    //! Rename a property with a new name.
    //void rename (const std::string & prop_key, const std::string & a_new_key);

    //! Erase property with a given key/name
    void erase(const std::string& key);

    //! Erase all properties.
    void erase_all();

    //! Erase all properties with key/name starting with prefix
    void erase_all_starting_with(const std::string& prefix);

    //! Erase all properties with key/name not starting with prefix
    void erase_all_not_starting_with(const std::string& prefix);

    //! Export all properties into another properties container
    void export_all(properties& a_props) const;

    //! Export all properties into another properties container adding a prefix
    void export_all_adding_prefix(properties& a_props, const std::string & prefix) const;

    //! Export all properties with key/name starting with a given prefix to another properties container
    void export_starting_with(properties& a_props,
                              const std::string& prop_key_prefix) const;

    //! Export all properties with key/name starting with a given prefix to another properties container but renaming the prefix on the fly
    void export_and_rename_starting_with(properties& a_props,
                                         const std::string& prop_key_prefix,
                                         const std::string& a_new_prefix) const;

    //! Export all properties with key/name not starting with a given suffix to another properties container
    void export_not_starting_with(properties& a_props,
                                  const std::string& prop_key_prefix) const;

    //! Export all properties with key/name fulfilling a given predicate
    template <class key_predicate>
    void export_if(properties& props, const key_predicate& predicate) const;

    //! Export all properties with key/name not fulfilling a given predicate
    template <class key_predicate>
    void export_not_if(properties& props,
                       const key_predicate& predicate) const;

    //! Erase all properties with key/name ending with a given suffix
    void erase_all_ending_with(const std::string& suffix);

    //! Erase all properties with key/name not ending with a given suffix
    void erase_all_not_ending_with(const std::string& suffix);

    //! Export all properties with key/name ending with a given suffix to another properties container
    void export_ending_with(properties& props,
                            const std::string& suffix) const;

    //! Export all properties with key/name not ending with a given suffix to another properties container
    void export_not_ending_with(properties& props,
                                const std::string& suffix) const;

    //! Remove a property with a given key/name
    void clean(const std::string& prop_key);

    //! Reset method (from the datatools::i_clear interface).
    virtual void clear();

    //! Reset method
    void reset();

    //! Set a boolean 'true' flag with a given key/name, a description string and a lock request
    void store_flag(const std::string& prop_key, const std::string& a_desc = "",
                    bool a_lock = false);

    //! Set a boolean 'true' flag with a given key/name
    /** @param prop_key The key of the boolean property
     */
    void set_flag(const std::string& prop_key);

    //! Remove a boolean flag with a given key/name
    /** @param prop_key The key of the boolean property
     */
    void unset_flag(const std::string& prop_key);

    //! Store a boolean property with a given key/name and value
    void store(const std::string& prop_key, bool a_value,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a boolean property with a given key/name and value
    void store_boolean(const std::string& prop_key, bool a_value,
                       const std::string& a_desc = "", bool a_lock = false);

    //! Store an integer property with a given key/name and value
    void store(const std::string& prop_key, int a_value,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store an integer property with a given key/name and value
    void store_integer(const std::string& prop_key, int a_value,
                       const std::string& a_desc = "",
                       bool a_lock = false);

    //! Store a real property with a given key/name and value
    void store(const std::string& prop_key, double a_value,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a real property with a given key/name and value
    void store_real(const std::string& prop_key, double a_value,
                    const std::string& a_desc = "", bool a_lock = false);

    //! Store a real property with a given key/name and value with the explicit unit flag
    /**
     *   \code
     *   datatools::properties config;
     *   config.store_real("box.zoom_factor", 2.5);
     *   config.store_real("box.efficiency", 0.17);
     *   config.store_real_with_explicit_unit("box.width", 3.4 * CLHEP::cm);
     *   config.store_real_with_explicit_unit("box.height", 5.2 * CLHEP::mm);
     *   \endcode
     */
    void store_real_with_explicit_unit(const std::string& prop_key, double a_value,
                                       const std::string& desc = "", bool a_lock = false);

    //! Store a real property with a given key/name and value with the explicit unit flag
    /// @see store_real_with_explicit_unit
    void store_with_explicit_unit(const std::string& prop_key, double a_value,
                                  const std::string& desc = "", bool a_lock = false);

    //! Set flag for explicit unit for a real property with a given key/name
    void set_explicit_unit(const std::string& prop_key, bool a_explicit_unit = true);

    //! Check flag for explicit unit for a real property with a given key/name
    bool has_explicit_unit(const std::string& prop_key) const;

    //! Set the unit symbol for a real property with a given key/name
    void set_unit_symbol(const std::string& prop_key, const std::string& unit_symbol = "");

    //! Check flag for unit symbol for a real property with a given key/name
    bool has_unit_symbol(const std::string& prop_key) const;

    //! Return the unit symbol for a real property with a given key/name
    const std::string & get_unit_symbol(const std::string& prop_key) const;

    //! Set flag for explicit path for a string property with a given key/name
    void set_explicit_path(const std::string& prop_key, bool a_explicit_path = true);

    //! Check flag for explicit path for a string property with a given key/name
    bool is_explicit_path(const std::string& prop_key) const;

    //! Store a string property with a given key/name and value
    void store(const std::string& prop_key, const std::string& a_value,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a string property with a given key/name and value
    void store_string(const std::string& prop_key, const std::string& a_value,
                      const std::string& a_desc = "", bool a_lock = false);

    //! Store a path property with a given key/name and value
    void store_path(const std::string& prop_key, const std::string& a_path_value,
                    const std::string& a_desc = "", bool a_lock = false);

    //! Store a string property with a given key/name and value (C style)
    void store(const std::string& prop_key, const char* a_value,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a boolean vector property with a given key/name and value
    void store(const std::string& prop_key, const data::vbool& a_values,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a integer vector property with a given key/name and value
    void store(const std::string& prop_key, const data::vint& a_values,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a real vector property with a given key/name and value
    void store(const std::string& prop_key, const data::vdouble& a_values,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a string vector property with a given key/name and value
    void store(const std::string& prop_key, const data::vstring& a_values,
               const std::string& a_desc = "", bool a_lock = false);

    //! Store a path string vector property with a given key/name and value
    void store_paths(const std::string& prop_key, const data::vstring& a_path_values,
                     const std::string& a_desc = "", bool a_lock = false);

    //! Change the value of an existing boolean property with a given key/name and index
    void change(const std::string& key, bool value, int index = 0);

    //! Change the value of an existing boolean property with a given key/name and index
    void change_boolean(const std::string& key, bool value, int index = 0);

    //! Change the value of an existing scalar boolean property with a given key/name
    void change_boolean_scalar(const std::string& key, bool value);

    //! Change the value of an existing vector boolean property with a given key/name and index
    void change_boolean_vector(const std::string& key, bool value, int index);

    //! Change the value of an existing integer property with a given key/name and index
    void change(const std::string& key, int value, int index = 0);

    //! Change the value of an existing integer property with a given key/name and index
    void change_integer(const std::string& key, int value, int index = 0);

    //! Change the value of an existing integer scalar property with a given key/name
    void change_integer_scalar(const std::string& key, int value);

    //! Change the value of an existing integer vector property with a given key/name and index
    void change_integer_vector(const std::string& key, int value, int index);

    //! Change the value of an existing real property with a given key/name and index
    void change(const std::string& key, double value, int index = 0);

    //! Change the value of an existing real property with a given key/name and index
    void change_real(const std::string& key, double value, int index = 0);

    //! Change the value of an existing real scalar property with a given key/name
    void change_real_scalar(const std::string& key, double value);

    //! Change the value of an existing real vector property with a given key/name and index
    void change_real_vector(const std::string& key, double value, int index);

    //! Change the value of an existing string property with a given key/name and index
    void change(const std::string& key, const std::string& value,
                int index = 0);

    //! Change the value of an existing string property with a given key/name and index
    void change_string(const std::string& key, const std::string& value,
                       int index = 0);

    //! Change the value of an existing string scalar property with a given key/name
    void change_string_scalar(const std::string& key, const std::string& value);

    //! Change the value of an existing string vector property with a given key/name and index
    void change_string_vector(const std::string& key, const std::string& value,
                              int index);

    //! Change the value of an existing string property (C style) with a given key/name and index
    void change(const std::string& key, const char* a_value, int index = 0);

    //! Change the full contents of an existing boolean vector property with a given key/name
    void change(const std::string& key, const data::vbool& values);

    //! Change the full contents of an existing integer vector property with a given key/name
    void change(const std::string& key, const data::vint& values);

    //! Change the full contents of an existing real vector property with a given key/name
    void change(const std::string& key, const data::vdouble& values);

    //! Change the full contents of an existing string vector property with a given key/name
    void change(const std::string& key, const data::vstring& values);

    //! Update a boolean flag to true with a given key/name
    void update_flag(const std::string& key);

    //! Update a boolean flag with a given key/name and value
    void update(const std::string& key, bool value);

    //! Update a boolean flag with a given key/name and value
    void update_boolean(const std::string& key, bool value);

    //! Update an integer flag with a given key/name and value
    void update(const std::string& key, int value);

    //! Update an integer flag with a given key/name and value
    void update_integer(const std::string& key, int value);

    //! Update a real flag with a given key/name and value
    void update(const std::string& key, double value);

    //! Update a real flag with a given key/name and value
    void update_real(const std::string& key, double value);

    //! Update a real flag with a given key/name and value
    void update_real_with_explicit_unit(const std::string& key, double value);

    //! Update a real flag with a given key/name and value
    void update_with_explicit_unit(const std::string& key, double value);

    //! Update a string flag with a given key/name and value
    void update(const std::string& key, const std::string& value);

    //! Update a C-string flag with a given key/name and value
    void update(const std::string& key, const char* value);

    //! Update a string flag with a given key/name and value
    void update_string(const std::string& key, const std::string& value);

    //! Update the full contents of a boolean vector with a given key/name
    void update(const std::string& key, const data::vbool& values);

    //! Update the full contents of an integer vector with a given key/name
    void update(const std::string& key, const data::vint& values);

    //! Update the full contents of a real vector with a given key/name
    void update(const std::string& key, const data::vdouble& values);

    //! Update the full contents of a string vector with a given key/name
    void update(const std::string& key, const data::vstring& values);

    //! Check if a boolean value with a given key/name exists with value 'true'
    bool has_flag(const std::string& key) const;

    //! Fetch the boolean value stored with a given key/name and index
    void fetch(const std::string& key, bool& value, int a_index = 0) const;

    //! Fetch the integer value stored with a given key/name and index
    void fetch(const std::string& key, int& value, int index = 0) const;

    //! Fetch the real value stored with a given key/name and index
    void fetch(const std::string& key, double& value, int index = 0) const;

    //! Fetch the string value stored with a given key/name and index
    void fetch(const std::string& key, std::string& value,
               int index = 0) const;

    //! Fetch the boolean vector value stored with a given key/name
    void fetch(const std::string& key, data::vbool& values) const;

    //! Fetch the integer vector value stored with a given key/name
    void fetch(const std::string& key, data::vint& values) const;

    //! Fetch the real vector value stored with a given key/name
    void fetch(const std::string& key, data::vdouble& values) const;

    //! Fetch the dimensionless real vector value stored with a given key/name
    void fetch_dimensionless(const std::string& key, data::vdouble& values) const;

    //! Fetch the string vector value stored with a given key/name
    void fetch(const std::string& key, data::vstring& values) const;

    //! Fetch a set of string values from the vector value stored with a given key/name
    void fetch(const std::string& key, std::set<std::string>& values, bool allow_duplication_ = false) const;

    //! Fetch a set of integer values from the vector value stored with a given key/name
    void fetch(const std::string& key, std::set<int>& values, bool allow_duplication_ = false) const;

    //! Fetch a set of unsigned integer values from the vector value stored with a given key/name
    void fetch_positive(const std::string& key, std::set<unsigned int>& values, bool allow_duplication_ = false) const;

    //! Fetch the boolean value stored with a given key/name and index
    bool fetch_boolean(const std::string&, int index = 0) const;

    //! Fetch the boolean scalar value stored with a given key/name
    bool fetch_boolean_scalar(const std::string& name) const;

    //! Fetch the boolean vector value stored with a given key/name and index
    bool fetch_boolean_vector(const std::string& name, int index) const;

    //! Fetch the integer value stored with a given key/name and index
    int fetch_integer(const std::string& name, int index = 0) const;

    //! Fetch the positive integer value stored with a given key/name and index
    unsigned int fetch_positive_integer(const std::string& name, int index = 0) const;

    //! Fetch the strict positive integer value stored with a given key/name and index
    unsigned int fetch_strict_positive_integer(const std::string& name, int index = 0) const;

    //! Fetch the ranged integer value stored with a given key/name and index
    int fetch_range_integer(const std::string& name, int min_, int max_, int index = 0) const;

    //! Fetch the integer scalar value stored with a given key/name
    int fetch_integer_scalar(const std::string& name) const;

    //! Fetch the integer vector value stored with a given key/name and index
    int fetch_integer_vector(const std::string& name , int index) const;

    //! Fetch the real value stored with a given key/name and index
    double fetch_real(const std::string& name, int index = 0) const;

    //! Fetch the dimensionless real value stored with a given key/name and index
    double fetch_dimensionless_real(const std::string& name, int index = 0) const;

    //! Fetch the physical quantity (with its explicit unit) value stored with a given key/name and index
    double fetch_real_with_explicit_unit(const std::string& name, int index = 0) const;

    //! Fetch the physical quantity (with its explicit dimension) value stored with a given key/name and index
    double fetch_real_with_explicit_dimension(const std::string& name, const std::string& dimension, int index = 0) const;

    //! Fetch the real scalar value stored with a given key/name
    double fetch_real_scalar(const std::string& name) const;

    //! Fetch the real vector value stored with a given key/name and index
    double fetch_real_vector(const std::string& name, int index) const;

    //! Fetch the string value stored with a given key/name and index
    std::string fetch_string(const std::string& name, int index = 0) const;

    //! Fetch a single character value stored with a given key/name and index
    char fetch_one_character(const std::string& name, int index = 0) const;

    //! Fetch the string scalar value stored with a given key/name
    std::string fetch_string_scalar(const std::string& name) const;

    //! Fetch the string vector value stored with a given key/name and index
    std::string fetch_string_vector(const std::string& name, int index) const;

    //! Fetch a file path from a string value stored with a given key/name and index
    std::string fetch_path(const std::string& name, int index = 0) const;

    //! Fetch a file path from a string scalar value stored with a given key/name
    std::string fetch_path_scalar(const std::string& name) const;

    //! Fetch a file path from a string vector value stored with a given key/name and index
    std::string fetch_path_vector(const std::string& name, int index) const;

    //! Basic print
    void dump(std::ostream& out = std::clog) const;

    //! Smart print
    virtual void tree_dump(std::ostream& a_out = std::clog,
                           const std::string& a_title  = "",
                           const std::string& a_indent = "",
                           bool a_inherit = false) const;

    std::string key_to_string(const std::string& key) const;

    std::string key_to_property_string(const std::string& key) const;

    void export_to_string_based_dictionary(
                                           std::map<std::string, std::string>& a_dict,
                                           bool a_quoted_strings = true) const;

    //! Store the properties' container object in an ASCII text file
    void write_configuration(const std::string& filename,
                             uint32_t options = config::SMART_MODULO | config::SKIP_PRIVATE) const;

    // //! @deprecated Store the properties' container object in an ASCII text file
    // void write_configuration(const std::string& filename,
    //                          bool a_use_smart_modulo,
    //                          bool a_write_public_only) const;

    //! Load the properties' container object from an ASCII text file
    void read_configuration(const std::string& filename,
                            uint32_t options = 0);

    //! Store the properties' container object in an ASCII text file
    static void write_config(const std::string& filename,
                             const properties& props,
                             uint32_t options = 0);

    // //! @deprecated Store the properties' container object in an ASCII text file
    // static void write_config(const std::string& filename,
    //                          const properties& props,
    //                          bool a_use_smart_modulo,
    //                          bool a_write_public_only);

    //! Load the properties' container object from an ASCII text file
    static void read_config(const std::string& filename,
                            properties& props,
                            uint32_t options = 0);

    //! Build a new property key from a prefix and a key
    static std::string build_property_key(const std::string& a_prefix,
                                          const std::string& a_subkey);

  protected:

    /// Default global key validator (singleton)
    static default_key_validator & global_default_key_validator();

    // methods:
  private:
    void _check_nokey_(const std::string& prop_key) const;

    void _check_key_(const std::string& prop_key, data** a_data);

    void _check_key_(const std::string& prop_key, const data** a_data) const;

    void _validate_key_(const std::string& prop_key) const;

    void _clear_key_validator_();

    // non static fields declarations:
  private:
    bool                        _debug_;                  //!< Debug flag
    std::string                 _description_;            //!< Description string
    pmap                        _props_;                  //!< Internal list of properties
    const basic_key_validator * _key_validator_;          //!< Reference to the embedded key validator
    bool                        _key_validator_deletion_; //!< Ownership flag for the embedded key validator

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(properties)

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(properties)
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

#ifndef Q_MOC_RUN
    //! Reflection interface
    DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };


  //----------------------------------------------------------------------
  // properties class template method definitions
  //
  template <class key_predicate>
  void properties::export_if(properties& props,
                             const key_predicate& predicate) const {
    DT_THROW_IF (this == &props,
                 std::logic_error, "Self export is not allowed !");
    keys_col_type ks;
    for (pmap::const_iterator iter = _props_.begin(); iter != _props_.end();
         ++iter) {
      if (predicate(iter->first)) {
        ks.push_back(iter->first);
      }
    }
    for (keys_col_type::const_iterator i = ks.begin(); i !=  ks.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      props._props_[*i] = ptmp._props_[*i];
    }
  }


  template <class key_predicate>
  void properties::export_not_if(properties& props,
                                 const key_predicate& predicate) const {
    DT_THROW_IF (this == &props,
                 std::logic_error, "Self export is not allowed !");
    keys_col_type ks;
    for (pmap::const_iterator iter = _props_.begin(); iter != _props_.end();
         ++iter) {
      if (!predicate(iter->first)) {
        ks.push_back(iter->first);
      }
    }
    for (keys_col_type::const_iterator i = ks.begin(); i != ks.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      props._props_[*i] = ptmp._props_[*i];
    }
  }

} // end of namespace datatools

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(::datatools::properties)
// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::datatools::properties)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/serialization/export.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

BOOST_CLASS_EXPORT_KEY2(datatools::properties, "datatools::properties")

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'datatools::properties' class:
DR_CLASS_INIT(::datatools::properties)
#endif // Q_MOC_RUN

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::properties::data, 2)
BOOST_CLASS_VERSION(datatools::properties, 2)

#endif // DATATOOLS_PROPERTIES_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --