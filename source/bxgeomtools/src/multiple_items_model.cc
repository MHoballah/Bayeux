// -*- mode: c++ ; -*-
/* multiple_items_model.cc
 */

#include <geomtools/multiple_items_model.h>

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <datatools/units.h>
#include <datatools/exception.h>

#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/utils.h>
#include <geomtools/visibility.h>
#include <geomtools/material.h>

namespace geomtools {

  using namespace std;

  MWIM & multiple_items_model::get_internals ()
  {
    return _internals_;
  }

  const MWIM & multiple_items_model::get_internals () const
  {
    return _internals_;
  }

  string multiple_items_model::get_model_id () const
  {
    return "geomtools::multiple_items_model";
  }

  const box & multiple_items_model::get_box () const
  {
    return _solid_;
  }

  const box & multiple_items_model::get_solid () const
  {
    return _solid_;
  }

  void multiple_items_model::set_material_name (const string & m_)
  {
    assert_unconstructed ("multiple_items_model::set_material_name");
    _material_name_ = m_;
    return;
  }

  const string & multiple_items_model::get_material_name () const
  {
    assert_constructed ("multiple_items_model::get_material_name");
    return _get_material_name_ ();
  }

  const string & multiple_items_model::_get_material_name_ () const
  {
    return _material_name_;
  }

  /*
    bool multiple_items_model::has_item (const string & label_) const
    {
    return __items.find (label_) != __items.end ();
    }

    const multiple_items_model::item &
    multiple_items_model::get_item (const string & label_) const
    {
    item_dict_t::const_iterator found = __items.find (label_);
    if (found == __items.end ())
    {
    ostringstream message;
    message << "multiple_items_model::get_item: "
    << "No item with label '" << label_ << "'"
    << endl;
    th row runtime_error (message.str ());
    }
    return found->second;
    }

    multiple_items_model::item &
    multiple_items_model::get_item (const string & label_)
    {
    item_dict_t::iterator found = __items.find (label_);
    if (found == __items.end ())
    {
    ostringstream message;
    message << "multiple_items_model::get_item: "
    << "No item with label '" << label_ << "' !"
    << endl;
    th row runtime_error (message.str ());
    }
    return found->second;
    }

    void multiple_items_model::add_item (const string & label_,
    const i_model & model_,
    const placement & placement_)
    {
    assert_unconstructed ("multiple_items_model::add_item");
    if (has_item (label_))
    {
    ostringstream message;
    message << "multiple_items_model::add_item: "
    << "There is already an item with label '" << label_ << "' !"
    << endl;
    th row runtime_error (message.str ());
    }

    __items[label_] = item ();
    item & the_item = __items[label_];
    the_item.set_label (label_);
    the_item.set_model (model_);
    the_item.set_placement (placement_);
    }

    size_t multiple_items_model::get_number_of_items () const
    {
    return __items.size ();
    }

    const multiple_items_model::item_dict_t &
    multiple_items_model::get_items () const
    {
    return __items;
    }

    multiple_items_model::item_dict_t & multiple_items_model::get_items ()
    {
    return __items;
    }

  */


  // ctor:
  multiple_items_model::multiple_items_model ()
    : i_boxed_model ()
  {
    return;
  }

  // dtor:
  multiple_items_model::~multiple_items_model ()
  {
    //__items.clear ();
    return;
  }

  void multiple_items_model::_at_construct (const string & name_,
                                            const datatools::properties & config_,
                                            models_col_type * models_)
  {
    set_name (name_);

    /*** box dimensions ***/
    double x, y, z;
    string length_unit = "mm";
    double lunit = CLHEP::mm;

    if (config_.has_key ("length_unit"))
      {
        string length_unit_str = config_.fetch_string ("length_unit");
        lunit = datatools::units::get_length_unit_from (length_unit_str);
      }

    if (config_.has_key ("x"))
      {
        x = config_.fetch_real ("x");
        if (! config_.has_explicit_unit ("x")) {
          x *= lunit;
        }
      }

    if (config_.has_key ("y"))
      {
        y = config_.fetch_real ("y");
        if (! config_.has_explicit_unit ("y")) {
          y *= lunit;
        }
      }

    if (config_.has_key ("z"))
      {
        z = config_.fetch_real ("z");
        if (! config_.has_explicit_unit ("z")) {
          z *= lunit;
        }
      }

    /*** material ***/
    {
      string material;
      DT_THROW_IF (! config_.has_key ("material.ref"),
                   logic_error,
                   "Missing 'material.ref' property !");
      material = config_.fetch_string ("material.ref");
      set_material_name (material);
    }

    _solid_.reset ();
    _solid_.set_x (x);
    _solid_.set_y (y);
    _solid_.set_z (z);
    DT_THROW_IF (!_solid_.is_valid (), logic_error, "Invalid solid !");

    get_logical ().set_name (i_model::make_logical_volume_name (name_));
    get_logical ().set_shape (_solid_);
    get_logical ().set_material_ref (_get_material_name_ ());

    _internals_.plug_internal_models (config_, get_logical (), models_);

    /*
      for (vector<string>::const_iterator i = labels.begin ();
      i != labels.end ();
      i++)
      {
      string item_label = *i;
      string item_model_name;
      const i_model * item_model = 0;
      placement item_placement;
      // extract placement:
      {
      string item_placement_str;
      ostringstream item_placement_oss;
      item_placement_oss << "placement." << item_label;
      if (config_.has_key (item_placement_oss.str ()))
      {
      item_placement_str = config_.fetch_string (item_placement_oss.str ());
      }
      else
      {
      ostringstream message;
      message << "multiple_items_model::_at_construct: "
      << "Missing '" << item_placement_oss.str () << "' property !";
      th row runtime_error (message.str ());
      }
      // parse placement:
      bool res_parsing = true;
      try
      {
      res_parsing = placement::from_string (item_placement_str, item_placement);
      }
      catch (...)
      {
      res_parsing = false;
      }
      if (! res_parsing)
      {
      ostringstream message;
      message << "multiple_items_model::_at_construct: "
      << "Item's placement parsing failed !";
      th row runtime_error (message.str ());
      }
      }

      // extract model:
      {
      ostringstream item_model_key_oss;
      item_model_key_oss << "model." << item_label;
      if (config_.has_key (item_model_key_oss.str ()))
      {
      item_model_name = config_.fetch_string (item_model_key_oss.str ());
      }
      else
      {
      ostringstream message;
      message << "multiple_items_model::_at_construct: "
      << "Missing 'item_model_key_oss.str ()' property !";
      th row runtime_error (message.str ());
      }
      {
      models_col_t::const_iterator found =
      models_->find (item_model_name);
      if (found != models_->end ())
      {
      item_model = found->second;
      }
      else
      {
      ostringstream message;
      message << "multiple_items_model::_at_construct: "
      << "Cannot find model with name '"
      << item_model_name << "' !";
      th row runtime_error (message.str ());
      }
      }
      }


      add_item (item_label, *item_model, item_placement);
      physical_volume & item_phys = get_item (item_label).get_physical_volume ();
      const placement & item_plcmt = get_item (item_label).get_placement ();
      item_phys.set_name (i_model::make_physical_volume_name (item_label));
      item_phys.set_placement (item_plcmt);
      item_phys.set_logical (item_model->get_logical ());
      item_phys.set_mother (get_logical ());
      //item_phys.tree_dump (clog, "Item phys: ", "DEVEL: ");

      //clog << "******* DEVEL: Item '" << item_label << "' is done..." << endl;
      }
    */

    return;
  }


  void multiple_items_model::tree_dump (ostream & out_,
                                        const string & title_,
                                        const string & indent_,
                                        bool inherit_) const
  {
    string indent;
    if (! indent_.empty()) indent = indent_;
    i_model::tree_dump (out_, title_, indent, true);

    {
      // Material:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Material : " << _get_material_name_ () << std::endl;
    }

    {
      // Items:
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Internal items : " << get_internals ().get_items ().size () << std::endl;
    }

    {
      // Solid:
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Solid : " << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        _solid_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    return;
  }

  // registration :
  GEOMTOOLS_MODEL_REGISTRATION_IMPLEMENT(multiple_items_model,"geomtools::multiple_items_model");

} // end of namespace geomtools

// end of multiple_items_model.cc
