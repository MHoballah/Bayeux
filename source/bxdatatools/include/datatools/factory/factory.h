/* datatools/factory/factory.h */
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-19
 * Last modified : 2012-04-30
 *
 */

#ifndef __datatools__factory__factory_h
#define __datatools__factory__factory_h 1

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <boost/scoped_ptr.hpp>
#include <datatools/utils/i_tree_dump.h>

namespace datatools {

  namespace factory {

    class base_factory_register
    {
    public:
      
      enum flag_type
        {
          verbose = 0x1,
        };
      
      base_factory_register ();
      
      virtual ~base_factory_register ();
      
    };
    
    /*********************************************/

    template <class BaseType>
    class factory_register : 
      public base_factory_register,
      public datatools::utils::i_tree_dumpable
    {
    public:

      typedef BaseType                            base_type;
      typedef boost::function<base_type*() >      factory_type;
      typedef std::map<std::string, factory_type> factory_map_type;

      bool is_verbose () const
      {
        return _verbose_;
      }

      void set_verbose (bool verbose_)
      {
        _verbose_ = verbose_;
        return;
      }

      const std::string & get_label () const
      {
        return _label_;
      }

      const std::string & label () const
      {
        return _label_;
      }

      void set_label (const std::string & label_)
      {
        _label_ = label_;
        return;
      }

      void list_of_factories (std::vector<std::string> & ids_) const
      {
        for (typename factory_map_type::iterator i = _registered_.begin ();
             i != _registered_.end ();
             i++)
          {
            ids_.push_back (i->first);
          }
        return;
      }

      /// Check if some factory with some given ID is registered
      bool has (const std::string & id_) const
      {
        return _registered_.find (id_) != _registered_.end ();
      }

      /// Clear all registered factories
      void clear ()
      {
        for (typename factory_map_type::iterator i = _registered_.begin ();
             i != _registered_.end ();
             i++)
          {
            if (i->second != 0)
              {
                if (is_verbose ())
                  {
                    std::clog << "INFO: "
                              << "datatools::factory::factory_register::clear: "
                              << "Destroying registered allocator/functor '"
                              << i->first << "'" << std::endl;
                  }
              }
          }
        _registered_.clear ();
        return;
      }

      /// Get a mutable reference to a factory given by registration ID
      factory_type & grab (const std::string & id_)
      {
        typename factory_map_type::iterator found = _registered_.find (id_);
        if (found == _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register::grab: "
                    << "Class ID '" << id_ << "' is not registered !";
            throw std::logic_error (message.str ());
          }
        return found->second;
      }

      /// Get a const reference to a factory given by registration ID
      const factory_type & get (const std::string & id_) const
      {
        typename factory_map_type::const_iterator found = _registered_.find (id_);
        if (found == _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register::get: "
                    << "Class ID '" << id_ << "' is not registered !";
            throw std::logic_error (message.str ());
          }
        return found->second;
      }

      /// Registration of a given factory with a given ID
      void registration (const std::string & id_, const factory_type & factory_)
      {
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">::registration: "
                      << "registration of class with ID '" << id_ << "'" << std::endl;
          }
        typename factory_map_type::const_iterator found = _registered_.find (id_);
        if (found != _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register<" << _label_ << ">::registration: "
                    << "Class ID '" << id_ << "' is already registered !";
            throw std::logic_error (message.str ());
          }
        _registered_[id_] = factory_;
        return;
      }

      /// Unregistration of the factory with a given ID
      void unregistration (const std::string & id_)
      {
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">::unregistration:  "
                      << "unregistration of class with ID '" << id_ << "'" << std::endl;
          }
        typename factory_map_type::const_iterator found = _registered_.find (id_);
        if (found == _registered_.end ())
          {
            std::ostringstream message;
            message << "datatools::factory::factory_register<" << _label_ << ">::unregistration: "
                    << "Class ID '" << id_ << "' is not registered !";
            throw std::logic_error (message.str ());
          }
        _registered_.erase (id_);
        return;
      };

      /// Constructor
      factory_register (const std::string & label_, unsigned int flags = 0x0)
      {
        _verbose_ = false;
        if (flags & verbose)
          {
            _verbose_ = true;
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << label_ << ">: Construction..." << std::endl;
          }
        _label_ = label_;
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">: Construction done." << std::endl;
          }
        return;
      }

      /// Destructor
      virtual ~factory_register ()
      {
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">: Destruction..." << std::endl;
          }
        clear ();
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">: Destruction done." << std::endl;
          }
        return;
      }

      /// Import registered factories from another factory register
      void import (const factory_register & factory_register_)
      {
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">::import: Importing registered factories from register '"
                      << factory_register_.label () << "'..." << std::endl;
          }
        for (typename factory_map_type::const_iterator i = factory_register_._registered_.begin ();
             i !=  factory_register_._registered_.end ();
             i++)
          {
            const factory_type & the_out_factory = i->second;
            registration (i->first, the_out_factory);
          }
        if (is_verbose ())
          {
            std::clog << "INFO: " << "datatools::factory::factory_register<"
                      << _label_ << ">::import: Done." << std::endl;
          }
        return;
      }

      /// Simple print
      void print (std::ostream & out_, const std::string & indent_ = "") const
      {
        std::ostringstream title_oss;
        title_oss << "List of registered allocators/functors for label \""
                  << _label_ << "\" : ";
        tree_dump (out_, title_oss.str (), indent_);
        return;
      }

      /// Smart print
      virtual void tree_dump (std::ostream & out_ = std::clog, 
                              const std::string & title_ = "", 
                              const std::string & indent_ = "",
                              bool inherit_              = false) const
      {
        namespace du = datatools::utils;
        std::string indent;
        if (! indent_.empty ()) indent = indent_;
        if (! title_.empty ())
          {
            out_ << indent << title_ << std::endl;
          }
        out_ << indent << du::i_tree_dumpable::tag
             << "Label   : '" << _label_ << "'" << std::endl;
        out_ << indent << du::i_tree_dumpable::tag 
             << "Verbose : " << _verbose_ << std::endl;
        out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
             << "Registered factories : " << std::endl;
        for (typename factory_map_type::const_iterator i = _registered_.begin ();
             i != _registered_.end ();
             i++)
          {
            out_ << indent_;
            out_ << du::i_tree_dumpable::inherit_skip_tag (inherit_);
            typename factory_map_type::const_iterator j = i;
            j++;
            if (j == _registered_.end ())
              {
                out_ << du::i_tree_dumpable::last_tag;
              }
            else
              {
                out_ << du::i_tree_dumpable::tag;
              }
            out_ << "ID: \"" <<  i->first << "\" @ " << &i->second << std::endl;
          }
        return;
      }

    private:

      std::string      _label_;      /// Label of the factory
      bool             _verbose_;    /// Verbosity flag
      factory_map_type _registered_; /// Dictionnary of registered factories

    };

  } // end of namespace factory

} // end of namespace datatools

#include <boost/type_traits/is_base_of.hpp>

namespace datatools {

  namespace factory {

    /* Utility to enable auto-(un)registration of the Type class in the
     * the system factory register:
     */
    template <class BaseType, class DerivedType>
    class _system_factory_registrator
    {
    private:

      void _trigger_factory_registration_ ()
      {
        if (! boost::is_base_of<BaseType, DerivedType>::value)
          {
            std::ostringstream message;
            message << "datatools::factory::_system_factory_registrator::_trigger_factory_registration_: "
                    << "Class ID '" << _type_id_ << "' cannot be registered in register '"
                    << BaseType::grab_system_factory_register ().label () << "' !";
            throw std::logic_error (message.str ());
          }
        BaseType::grab_system_factory_register ().registration (_type_id_,
                                                                boost::factory<DerivedType*>());
        return;
      }
 
      void _trigger_factory_unregistration_ ()
      {
        if (BaseType::grab_system_factory_register ().has (_type_id_))
          {
            BaseType::grab_system_factory_register ().unregistration (_type_id_);
          }
        return;
      }
 
    public:

      _system_factory_registrator (const std::string & type_id_)
      {
        _type_id_ = type_id_;
        _trigger_factory_registration_ ();
        return;
      }

      ~_system_factory_registrator ()
      {
        _trigger_factory_unregistration_ ();
        return;
      }

    private :

      std::string _type_id_;

    };

  } // end of namespace factory

} // end of namespace datatools

#endif // __datatools__utils__factory_h

/* end of datatools/utils/factory.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
