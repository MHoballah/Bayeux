// datatools/urn_query_service.cc
//
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <datatools/urn_query_service.h>

// Standard Library:
#include <iostream>
#include <map>
#include <mutex>
#include <regex>

// This project:
#include <datatools/urn_db_service.h>
#include <datatools/urn_to_path_resolver_service.h>

namespace datatools {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(urn_query_service, "datatools::urn_query_service")

  typedef std::map<std::string, const urn_db_service*> dbs_dict_type;
  typedef std::map<std::string, const urn_to_path_resolver_service*> resolvers_dict_type;

  struct urn_query_service::pimpl_type
  {
    pimpl_type();
    dbs_dict_type       dbs;
    resolvers_dict_type resolvers;
    std::mutex          mtx;
  };

  urn_query_service::pimpl_type::pimpl_type()
  {
    // std::cerr << "urn_query_service::pimpl_type::pimpl_type: Hello PIMPL!" << std::endl;
  }

  urn_query_service::urn_query_service()
  {
    _pimpl_.reset(new pimpl_type);
  }

  urn_query_service::~urn_query_service()
  {
    if (this->is_initialized()) this->urn_query_service::reset();
    _pimpl_.reset();
  }

  bool urn_query_service::is_initialized() const
  {
    return _initialized_;
  }

  int urn_query_service::initialize(const datatools::properties & config_,
                             datatools::service_dict_type& /*services_*/)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "URN service is already initialized!");
    base_service::common_initialize(config_);
    _initialized_ = true;
    return datatools::SUCCESS;
  }

  int urn_query_service::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "URN service is not initialized!");
    _initialized_ = false;
    return datatools::SUCCESS;
  }

  /*
  // static
  urn_query_service & urn_query_service::_instance_()
  {
    std::cerr << "devel **** urn_query_service::_instance_ **** entering..." << std::endl;
    static std::unique_ptr<urn_query_service> _singleton;
    if (_singleton.get() == nullptr) {
      std::cerr << "devel **** urn_query_service::_instance_ **** Instantiating the singleton URN system..." << std::endl;
      _singleton.reset(new urn_query_service());
      _singleton->set_name("bxKernUrnSys");
      _singleton->set_display_name("BxKernelUrnSystem");
      _singleton->set_terse_description("Kernel URN system");
      _singleton->initialize_simple();
      _singleton->tree_dump(std::cerr, "Bayeux/datatoools Kernel's URN system singleton: ", "[devel] ");
      std::cerr << "devel **** urn_query_service::_instance_ **** The singleton URN system has been instantiated." << std::endl;
    }
    std::cerr << "devel **** urn_query_service::_instance_ **** exiting." << std::endl;
    return *_singleton.get();
  }
  */

  // // static
  // urn_query_service & urn_query_service::instance()
  // {
  //   std::cerr << "devel **** urn_query_service::instance **** entering..." << std::endl;
  //   return _instance_();
  // }

  // // static
  // const urn_query_service & urn_query_service::const_instance()
  // {
  //   return const_cast<urn_query_service&>(_instance_());
  // }

  // virtual
  void urn_query_service::tree_dump(std::ostream& out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    this->base_service::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "URN database services: [" << _pimpl_->dbs.size() << ']' << std::endl;
    for (dbs_dict_type::const_iterator i = _pimpl_->dbs.begin();
         i != _pimpl_->dbs.end();
         i++) {
      dbs_dict_type::const_iterator j = i;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (++j == _pimpl_->dbs.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Database '" << i->first << "' (#URNs=" << i->second->get_number_of_urn_infos() << ')' << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "URN path resolver services: [" << _pimpl_->resolvers.size() << ']' << std::endl;
    for (resolvers_dict_type::const_iterator i = _pimpl_->resolvers.begin();
         i != _pimpl_->resolvers.end();
         i++) {
      resolvers_dict_type::const_iterator j = i;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (++j == _pimpl_->resolvers.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Path resolver service '" << i->first << "' (#URNs=" << i->second->size() << ')'  << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Address: [@" << this << ']' << std::endl;

    return;
  }

  const urn_info & urn_query_service::get_urn_info(const std::string & urn_) const
  {
    for (dbs_dict_type::const_iterator i = _pimpl_->dbs.begin();
         i != _pimpl_->dbs.end();
         i++) {
      if (i->second->has(urn_)) {
        const urn_info & ui = i->second->get(urn_);
        return ui;
      }
    }
    DT_THROW(std::logic_error, "No registered URN info '" << urn_ << "'!");
  }

  bool urn_query_service::urn_exists(const std::string & urn_,
                              const std::string & category_) const
  {
    for (dbs_dict_type::const_iterator i = _pimpl_->dbs.begin();
         i != _pimpl_->dbs.end();
         i++) {
      if (i->second->has(urn_)) {
        const urn_info & ui = i->second->get(urn_);
        if (category_.empty() || category_ == ui.get_category()) {
          return true;
        }
      }
    }
    return false;
  }

  bool urn_query_service::urn_find(std::vector<std::string> & urn_list_,
                            const std::string & urn_db_regex_,
                            const std::string & urn_regex_,
                            const std::string & urn_category_regex_,
                            bool clear_) const
  {
    bool found_more = false;
    std::string last_regex;
    try  {
      if (clear_) urn_list_.clear();
      std::string urn_db_regex = urn_db_regex_;
      std::string urn_regex    = urn_regex_;
      std::string urn_category_regex = urn_category_regex_;
      DT_LOG_DEBUG(get_logging_priority(), "Number of URN databases to be scanned : [" << _pimpl_->dbs.size() << "]");
      for (dbs_dict_type::const_iterator i = _pimpl_->dbs.begin();
           i != _pimpl_->dbs.end();
           i++) {
        DT_LOG_DEBUG(get_logging_priority(), "Searching in URN database '" << i->first << "'...");
        if (!urn_db_regex.empty()) {
          // Not a valid DB name:
          last_regex = urn_db_regex;
          std::regex urndbexp(urn_db_regex);
          DT_LOG_TRACE(get_logging_priority(), "Checking URN database regex '" << last_regex << "'...");
          if (!std::regex_match(i->first, urndbexp)) {
            DT_LOG_TRACE(get_logging_priority(), "URN database '" << i->first << "' does not match with '" << urn_db_regex << "'!");
            continue;
          }
        }
        const urn_db_service & db = *i->second;
        for (urn_db_service::urn_info_dict_type::const_iterator j = db.get_urn_infos().begin();
             j != db.get_urn_infos().end();
             j++) {
          DT_LOG_TRACE(get_logging_priority(), "Checking URN '" << j->first << "'...");
          const urn_info & ui = j->second;
          bool accept = true;
          if (!urn_regex.empty()) {
            // Not a valid URN name:
            last_regex = urn_regex;
            std::regex urnexp(urn_regex);
            DT_LOG_TRACE(get_logging_priority(), "Checking URN regex '" << last_regex << "'...");
            if (!std::regex_match(ui.get_urn(), urnexp)) {
              accept = false;
            }
          }
          if (!urn_category_regex.empty()) {
            // Not a valid URN category:
            last_regex = urn_category_regex;
            std::regex catexp(urn_category_regex);
            DT_LOG_TRACE(get_logging_priority(), "Checking URN category regex '" << last_regex << "'...");
            if (!std::regex_match(ui.get_category(), catexp)) {
              accept = false;
            }
          }
          if (accept) {
            urn_list_.push_back(j->first);
            found_more = true;
          }
        }
      }
    } catch (std::exception & error) {
      DT_THROW(std::logic_error, "URN search met an error: " << error.what() << " (last regex='" << last_regex << "')!");
      return false;
    }
    return found_more;
  }

  bool urn_query_service::urn_has_path(const std::string & urn_) const
  {
    for (resolvers_dict_type::const_iterator i = _pimpl_->resolvers.begin();
         i != _pimpl_->resolvers.end();
         i++) {
      if (i->second->urn_is_known(urn_)) return true;
    }
    return false;
  }

  bool urn_query_service::resolve_urn_as_path(const std::string & urn_,
                                       std::string & category_,
                                       std::string & mime_,
                                       std::string & path_) const
  {
    for (resolvers_dict_type::const_iterator i = _pimpl_->resolvers.begin();
         i != _pimpl_->resolvers.end();
         i++) {
      const urn_to_path_resolver_service & upr = *i->second;
      if (upr.urn_is_known(urn_)) {
        std::string cat = upr.get_category(urn_);
        std::string mime = upr.get_mime(urn_);
        if (!category_.empty()) {
          std::regex catexp(category_);
          if (!std::regex_match(cat, catexp)) {
            return false;
          }
        }
        if (!mime_.empty()) {
          std::regex mimeexp(mime_);
          if (!std::regex_match(mime, mimeexp)) {
            return false;
          }
        }
        category_ = cat;
        mime_ = mime;
        path_ = upr.get_path(urn_);
        return true;
      }
    }
    return false;
  }

  bool urn_query_service::has_db(const std::string & name_) const
  {
    return _pimpl_->dbs.count(name_) > 0;
  }

  bool urn_query_service::has_db(const urn_db_service & dbs_) const
  {
    for (auto dbsp : _pimpl_->dbs) {
      if (dbsp.second == &dbs_) {
        return true;
      }
    }
    return false;
  }

  void urn_query_service::add_db(const urn_db_service & dbs_,
                          const std::string & name_)
  {
    std::lock_guard<std::mutex> lck(_pimpl_->mtx);
    std::string name = name_;
    if (name.empty()) {
      name = dbs_.get_name();
    }
    if (has_db(name)) {
      DT_LOG_ERROR(get_logging_priority(),
                   "An URN database service with name '" << name << "' already exists!");
      return;
    }
    if (has_db(dbs_)) {
      DT_LOG_WARNING(get_logging_priority(),
                     "URN database service '" << dbs_.get_name() << "' already exists!");
      return;
    }
    DT_LOG_DEBUG(get_logging_priority(),
                 "Adding URN database service '" << name << "'...");
    _pimpl_->dbs[name] = &dbs_;
    return;
  }

  void urn_query_service::remove_db(const std::string & name_)
  {
    std::lock_guard<std::mutex> lck(_pimpl_->mtx);
    DT_LOG_DEBUG(get_logging_priority(),
                 "Removing URN database service '" << name_ << "'...");
    _pimpl_->dbs.erase(name_);
    return;
  }

  void urn_query_service::remove_db(const urn_db_service & dbs_)
  {
    std::lock_guard<std::mutex> lck(_pimpl_->mtx);
    for (auto db : _pimpl_->dbs) {
      if (db.second == &dbs_) {
        DT_LOG_DEBUG(get_logging_priority(),
                     "Removing URN database service '" << db.first << "'...");
        _pimpl_->dbs.erase(db.first);
        break;
      }
    }
    return;
  }

  bool urn_query_service::has_path_resolver(const std::string & name_) const
  {
    return _pimpl_->resolvers.count(name_) > 0;
  }

  bool urn_query_service::has_path_resolver(const urn_to_path_resolver_service & prs_) const
  {
    for (const auto & prsp : _pimpl_->resolvers) {
      // std::cerr << "devel **** urn_query_service::has_path_resolver **** checking: '" << prsp.second << "'" << std::endl;
      if (prsp.second == &prs_) {
        return true;
      }
    }
    return false;
  }

  void urn_query_service::add_path_resolver(const urn_to_path_resolver_service & prs_,
                                     const std::string & name_)
  {
    std::lock_guard<std::mutex> lck(_pimpl_->mtx);
    std::string name = name_;
    if (name.empty()) {
      name = prs_.get_name();
    }
    if (has_path_resolver(name_)) {
      DT_LOG_WARNING(get_logging_priority(),
                     "An URN path resolver service with name '" << name << "' already exists!");
      return;
    }
    if (has_path_resolver(prs_)) {
      DT_LOG_WARNING(get_logging_priority(),
                     "URN path resolver service '" << prs_.get_name() << "' already exists!");
      return;
    }
    DT_LOG_DEBUG(get_logging_priority(),
                 "Adding URN path resolver service '" << name << "'...");
    _pimpl_->resolvers[name] = &prs_;
    return;
  }

  void urn_query_service::remove_path_resolver(const std::string & name_)
  {
    std::lock_guard<std::mutex> lck(_pimpl_->mtx);
    DT_LOG_DEBUG(get_logging_priority(),
                 "Removing URN path resolver service '" << name_ << "'...");
    _pimpl_->resolvers.erase(name_);
    return;
  }

  void urn_query_service::remove_path_resolver(const urn_to_path_resolver_service & prs_)
  {
    std::lock_guard<std::mutex> lck(_pimpl_->mtx);
    for (auto prs : _pimpl_->resolvers) {
      if (prs.second == &prs_) {
        DT_LOG_DEBUG(get_logging_priority(),
                     "Removing URN path resolver service '" << prs.first << "'...");
        _pimpl_->resolvers.erase(prs.first);
        break;
      }
    }
    return;
  }

} // end of namespace datatools
