// mygsl::multidimensional_minimization.cc

#include <mygsl/multidimensional_minimization.h>

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <limits>

namespace mygsl {

  const size_t multidimensional_minimization::DEFAULT_MAX_ITER    = 1000;
  const size_t multidimensional_minimization::DEFAULT_MODULO_ITER = 100;
  const bool   multidimensional_minimization::DEFAULT_VERBOSE  = true;
  const double multidimensional_minimization::DEFAULT_EPSABS   = 1.e-3;
  bool         multidimensional_minimization::g_debug          = false;

  const double multidimensional_minimization_system::DEFAULT_OUT_OF_LIMIT_SLOPE = 1.e10;

  /************************************************************************/

  void multidimensional_minimization_system::set_numeric_eval_df (bool use_)
  {
    _numeric_eval_df_ = use_;
    return;
  }

  bool multidimensional_minimization_system::is_numeric_eval_df () const
  {
    return _numeric_eval_df_;
  }

  multidimensional_minimization_system::multidimensional_minimization_system (double slope_,
                                                                              bool use_numeric_eval_)
  {
    _out_of_limit_slope_ = slope_;
    if (slope_ <= 0)
      {
        _out_of_limit_slope_ = DEFAULT_OUT_OF_LIMIT_SLOPE;
      }
    _numeric_eval_df_ = use_numeric_eval_;
    return;
  }

  multidimensional_minimization_system::~multidimensional_minimization_system ()
  {
    return;
  }

  double multidimensional_minimization_system::func_eval_f_MR  (double x_, void * params_)
  {
    bool local_devel = multidimensional_minimization::g_debug;
    //if (local_devel) clog << "DEVEL: func_eval_f: entering... " << endl;
    func_eval_f_param * fefp = static_cast<func_eval_f_param *>(params_);
    int free_param_index = fefp->free_param_index;
    multidimensional_minimization_system * sys = fefp->sys;

    // backup former 'x' value:
    double save_x = sys->get_free_param (free_param_index).get_value ();

    // enforce 'x' value:
    sys->get_free_param (free_param_index).set_value_no_check (x_);
    sys->init_params_values ();

    double result = numeric_limits<double>::quiet_NaN ();
    if (sys->_eval_f_MR_ (result) != 0)
      {
        throw runtime_error ("multidimensional_minimization_system::func_eval_f: Cannot evaluate function!");
      }

    // restore former 'x' value:
    sys->get_free_param (free_param_index).set_value_no_check (save_x);
    sys->init_params_values ();
    //if (local_devel) clog << "DEVEL: func_eval_f: exiting." << endl;
    return result;
  }

  int multidimensional_minimization_system::_eval_f_MR_ (double & f_)
  {
    bool local_debug = multidimensional_minimization::g_debug;
    int code = _eval_f (f_);
    if (code != 0)
      {
        return code;
      }

    // search for 'out of limits' free parameters: 
    double slope = _out_of_limit_slope_;
    bool dump = false;
    for (int i = 0; i < get_number_of_free_params (); i++) 
      {
        double dist = get_free_param (i).get_dist_to_limit ();
        double step = get_free_param (i).get_step ();
        // add arbitrary huge value:
        double df = slope * dist / step;
        if (local_debug)
          {
            if (df != 0.0)
              {
                clog << "DEVEL: multidimensional_minimization_system::_eval_f_MR_: out of limits == " << get_free_param (i).get_name () << " : Delta(f) == " << df << endl;
                dump = true; //print_status (clog);
              }
          }
        f_ += df;
      }

    // search for 'out of limits' auto parameters: 
    for (int i = 0; i < get_number_of_auto_params (); i++) 
      {
        double dist = get_auto_param (i).get_dist_to_limit ();
        double step = get_auto_param (i).get_step ();
        // add arbitrary huge value:
        double df = slope * dist / step;
        if (local_debug)
          {
            if (df != 0.0)
              {
                clog << "DEVEL: multidimensional_minimization_system::_eval_f_MR_: out of limits == " << get_auto_param (i).get_name () << " : Delta(f) == " << df << endl;
                dump = true; //print_status (clog);
              }
          }
        f_ += df;
      }
    if (dump) print_status (clog);
    return code;
  }

  int multidimensional_minimization_system::_eval_df_MR_ (double * gradient_)
  {
    int code = _eval_df (gradient_);
    if (code != 0)
      {
        return code;
      }

    // search for 'out of limits' free parameters: 
    double slope = _out_of_limit_slope_;
    for (int i = 0; i < get_number_of_free_params (); i++) 
      {
        double sign = get_free_param (i).get_sign_limit ();
        // add arbitrary derivative value:
        gradient_[i] += sign * _out_of_limit_slope_;
      }

    // search for 'out of limits' auto parameters: 
    for (int i = 0; i < get_number_of_auto_params (); i++) 
      {
        double sign = get_auto_param (i).get_sign_limit ();
        // add arbitrary derivative value:
        gradient_[i] += sign * _out_of_limit_slope_;
      }

    return code;
  }

  int multidimensional_minimization_system::_numerical_eval_df_MR_ (double * gradient_)
  {
    // clog << "DEVEL: MMS::_numerical_eval_df_MR_" << endl;
    bool local_devel = multidimensional_minimization::g_debug;
    if (local_devel) clog << "DEVEL: __numerical_eval_df: entering..." << endl;  
    for (int i = 0; i < get_number_of_free_params (); i++) 
      {
        double save_value = get_free_param (i).get_value ();
        double x = save_value;
        double h = get_free_param (i).get_step ();
        double df = 0.0;
        double df_err = 0.0;
        int res_deriv = GSL_SUCCESS;
        gsl_function func;
        func_eval_f_param fefp (i, *this);
        func.params = &fefp;
        func.function = multidimensional_minimization_system::func_eval_f_MR;
        if (get_free_param (i).is_in_safe_range ())
          {
            if (local_devel) clog << "DEVEL: __numerical_eval_df: gsl_deriv_central... " << endl;
            res_deriv = gsl_deriv_central (&func, x, h, &df, &df_err);
          }
        else if (get_free_param (i).is_in_range_but_close_to_min ())
          {
            if (local_devel) clog << "DEVEL: __numerical_eval_df: gsl_deriv_forward... " << endl;
            res_deriv = gsl_deriv_forward (&func, x, h, &df, &df_err);
          }
        else if (get_free_param (i).is_in_range_but_close_to_max ())
          {
            if (local_devel) clog << "DEVEL: __numerical_eval_df: gsl_deriv_backward... " << endl;
            res_deriv = gsl_deriv_backward (&func, x, h, &df, &df_err);
          }
        else 
          {
            df = 0.0;
          }
        gradient_[i] = df;
      }
    if (local_devel) clog << "DEVEL: __numerical_eval_df: exiting." << endl;  
    return 0;
  }

  int multidimensional_minimization_system::_eval_df (double * gradient_)
  {
    cerr << "ERROR: The 'multidimensional_minimization_system::_eval_df' method " << endl;
    cerr << "       must be overloaded for your own class!" << endl;
    cerr << "       If not, you should consider to activate the " << endl;
    cerr << "       'use_numeric_eval_df(true)' method in order to perform" << endl;
    cerr << "       automatically the numerical estimation of the " << endl;
    cerr << "       derivatives!" << endl;
    return 1;
  }
 
  double multidimensional_minimization_system::get_out_of_limit_slope () const
  {
    return _out_of_limit_slope_;
  }

  int multidimensional_minimization_system::eval_f (const double * x_, 
                                                    double & f_)
  {
    // clog << "DEVEL: MMS::eval_f" << endl;
    from_double_star (x_, get_number_of_free_params ());
    int code = _eval_f_MR_ (f_);
    return code;
  }

  int multidimensional_minimization_system::eval_df (const double * x_, 
                                                     double * gradient_)
  {
    // clog << "DEVEL: MMS::eval_df" << endl;
    from_double_star (x_, get_number_of_free_params ());
    int code = 0;
    if (! _numeric_eval_df_)
      {
        code = _eval_df_MR_ (gradient_);
      }
    else
      {
        code = multidimensional_minimization_system::_numerical_eval_df_MR_ (gradient_);
      }
    return code;
  }

  int multidimensional_minimization_system::eval_fdf (const double * x_, 
                                                      double & f_, 
                                                      double * gradient_)
  {
    // clog << "DEVEL: MMS::eval_fdf" << endl;
    int code = eval_f (x_, f_);
    if (code != 0) 
      {
        return code;
      }
    return eval_df (x_, gradient_);
  }


  int multidimensional_minimization_system::init_params_values ()
  {
    //lock_params ();
    multiparameter_system::init ();
    return 0;
  }
  
  void multidimensional_minimization_system::to_double_star (double * pars_, 
                                                             size_t dimension_) const
  {
    size_t fd = get_number_of_free_params ();
    if (dimension_ != fd) 
      {
        throw std::range_error ("multidimensional_minimization_system::to_double_star: Invalid dimension!");
      }
    int i_free = 0;
    for (int i = 0; i < fd; i++) 
      { 
        pars_[i] = get_free_param (i).get_value ();
      }
    return;
  }

  void multidimensional_minimization_system::from_double_star (const double * pars_, 
                                                               size_t dimension_)
  {
    if (dimension_ != get_number_of_free_params ()) 
      {
        throw std::range_error ("multidimensional_minimization_system::from_double_star: Invalid dimension!");
      }
    for (int i = 0; i < get_number_of_free_params (); i++) 
      { 
        get_free_param (i).set_value_no_check (pars_[i]);
      }
    init_params_values ();
    return;
  }

  /*
    void multidimensional_minimization_system::search_likelihood_sigma ()
    {
    for (int param_index = param_begin; 
    param_index < param_end; 
    param_index++)
    {
    const mygsl::param_entry & param = get_free_param (param_index);
    double h  = param.get_step ();
    mygsl::multidimensional_minimization_system::func_eval_f_param func;
    func.free_param_index = param_index;
    func.sys              = const_cast<mygsl::multidimensional_minimization_system*>(this);
    double sigma_low;
    double sigma_high;
    double p = param.get_value ();
    double fmin = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
    double h = param.get_step ();
    double dir = +1;
    while (true)
    { 
    p += (dir * h);
    double f 
    = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
    if (f > (fmin + 0.5))
    {
    dir = -1;
    h /= 20;
    }
    }
    while (
    }
    }
  */

  void multidimensional_minimization_system::plot_f (const string & prefix_, int mode_) const
  {
    string prefix = prefix_;
    string ext = ".data";
    if (prefix.empty ())
      {
        prefix = "mms_param_";
      }
    int param_begin = 0;
    int param_end = get_number_of_free_params ();
    for (int param_index = param_begin; 
         param_index < param_end; 
         param_index++)
      {
        const mygsl::param_entry & param = get_free_param (param_index);
        clog << "NOTICE: multidimensional_minimization_system::plot_f: " 
             << "Plot function to be minimized for parameter '" 
             << param.get_name () 
             << "' in the current parameters' region: " << endl;
        ostringstream fname; 
        fname << prefix  
              << param.get_name () << ext;
        ofstream f_param (fname.str ().c_str ());
        int count = 0;
        double h  = param.get_step ();
        double h2 = h / 50.;
        double dt = h;
        double min = param.get_min ();
        double max = param.get_max ();
        for (double p = (min - 5.5 * h);
             p < (max + 5.5 * h);
             p += dt)
          {
            if (p < (min - 1.5 * h))
              {
                dt = h;
              } 
            else if (p < (min + h))
              {
                dt = h2;
              } 
            else if (p < (max - 1.5 * h )) 
              {
                dt = h;
              }
            else if (p < (max + h )) 
              {
                dt = h2;
              }
            else
              {
                dt = h;
              }
            mygsl::multidimensional_minimization_system::func_eval_f_param 
              func (param_index, const_cast<mygsl::multidimensional_minimization_system&>(*this));
            //func.free_param_index = param_index;
            //func.sys              = const_cast<mygsl::multidimensional_minimization_system*>(this);
            double f 
              = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
            f_param.precision (14);
            f_param << p << ' ' << f << endl;
            //if ((count % 100) == 0) clog << "  Count #" << count << endl;
            count++;
          }
        f_param.close ();
        clog << "NOTICE: multidimensional_minimization_system::plot_f: " 
             << "done." << endl;
      }
    return;
  }
 
  /**************************************************************************/
 
  double multidimensional_minimization::f (const gsl_vector * v_, 
                                           void * params_)
  {
    multidimensional_minimization_system * mms 
      = static_cast<multidimensional_minimization_system*> (params_);
    double f;
    mms->eval_f (v_->data, f);
    return f;
  }
  
  void multidimensional_minimization::df (const gsl_vector * v_, 
                                          void * params_, 
                                          gsl_vector * gradient_)
  {
    multidimensional_minimization_system * mms 
      = static_cast<multidimensional_minimization_system*> (params_);
    mms->eval_df (v_->data, gradient_->data);
    return;
  }

  void multidimensional_minimization::fdf (const gsl_vector * v_, 
                                           void * params_, 
                                           double * f_, 
                                           gsl_vector * gradient_)
  {
    multidimensional_minimization_system * mms 
      = static_cast<multidimensional_minimization_system*> (params_);
    mms->eval_fdf (v_->data, *f_, gradient_->data);
    return;
  }

  /****************************************************************************/

  multidimensional_minimization::default_step_action multidimensional_minimization::__default_step_action;

  void multidimensional_minimization::set_default_step_action ()
  {
    _at_step_action_ = &multidimensional_minimization::__default_step_action;
    return;
  }

  void multidimensional_minimization::unset_step_action ()
  {
    _at_step_action_ = 0;
    return;
  }

  void multidimensional_minimization::set_step_action (multidimensional_minimization::at_step_action & asd_)
  {
    _at_step_action_ = &asd_;
    return;
  }
  
  void multidimensional_minimization::default_step_action::action (int status_, 
                                                                   size_t iter_,
                                                                   double * x_, 
                                                                   size_t   dim_,
                                                                   double   f_)
  {
    int    status = status_;
    size_t iter   = iter_;
    double * x    = x_;
    size_t dim    = dim_;
    double   f    = f_;
    if (g_debug) 
      {
        std::cerr << "DEBUG: Iteration: " << iter << " (" 
                  << ((status == GSL_SUCCESS)? "minimum found": "continue") 
                  << ')' << std::endl;
      }
    std::cout << iter << ' ' << dim << ' ';
    for (int i = 0; i < dim ; i++) 
      {
        std::cout.precision (15);
        std::cout << x[i] << ' ';
      } 
    std::cout << f << ' ' << status << ' ';
    std::cout << std::endl;
    return;
  }

  /**********************************************************/

  void multidimensional_minimization::print_types (std::ostream & out_)
  {
    out_ << "conjugate_fr" << ' ';
    out_ << "conjugate_pr" << ' ';
    out_ << "vector_bfgs" << ' ';
    out_ << "steepest_descent" << ' ';
    out_ << "nmsimplex";
    return;
  }

  bool multidimensional_minimization::name_is_valid (const std::string & name_)
  {
    if (name_ == "conjugate_fr") return true;
    if (name_ == "conjugate_pr") return true;
    if (name_ == "vector_bfgs" ) return true;
    if (name_ == "steepest_descent") return true;
    if (name_ == "nmsimplex"  ) return true;
    return false;
  }

  std::string multidimensional_minimization::get_name () const
  {
    if (_mode_ == MODE_FDF && _fdfmin_ != 0 ) 
      {
        return std::string (gsl_multimin_fdfminimizer_name (_fdfmin_));
      }
    if (_mode_ == MODE_F && _fmin_ != 0) 
      {
        return std::string (gsl_multimin_fminimizer_name (_fmin_));
      }
    return "";
  }

  void multidimensional_minimization::_init_algorithm_ (const std::string & name_) 
  {
    if (! name_is_valid (name_)) 
      {
        std::ostringstream message;
        message << "multidimensional_minimization::_init_algorithm_: "
                << "Invalid minimization algorithm '" 
                << name_ << "'!";
        throw std::runtime_error (message.str ());
      }
    
    if (name_ == "conjugate_fr") 
      {
        _algo_fdf_ = gsl_multimin_fdfminimizer_conjugate_fr;
        _set_mode_ (MODE_FDF);
      }
    
    if (name_ == "conjugate_pr") 
      {
        _algo_fdf_ = gsl_multimin_fdfminimizer_conjugate_pr;
        _set_mode_ (MODE_FDF);
      }
    
    if (name_ == "vector_bfgs") 
      {
        _algo_fdf_ = gsl_multimin_fdfminimizer_vector_bfgs;
        _set_mode_ (MODE_FDF);
      }
    
    if (name_ == "steepest_descent") 
      {
        _algo_fdf_ = gsl_multimin_fdfminimizer_steepest_descent;
        _set_mode_ (MODE_FDF);
      }
    
    if (name_ == "nmsimplex") 
      {
        _algo_f_ = gsl_multimin_fminimizer_nmsimplex;
        _set_mode_ (MODE_F);
      }
    return;
  }

  void multidimensional_minimization::_set_mode_ (int mode_)
  {
    if (mode_ != MODE_F && mode_ != MODE_FDF) 
      {
        throw std::runtime_error ("multidimensional_minimization:_set_mode_: Invalid mode!");
      }
    _mode_ = mode_;
    return;
  }

  void multidimensional_minimization::devel_dump_x () const
  {
    if (_x_ == 0) 
      {
        std::cerr << "DEVEL: _x_==0" << std::endl;
        return;
      }
    std::cerr << "DEVEL ================================= " << std::endl; 
    std::cerr << "DEVEL: " 
              << " _x_.size  = " << _x_->size
              << std::endl;
    std::cerr << "DEVEL: " 
              << " _x_.stride  = " << _x_->stride
              << std::endl;
    std::cerr << "DEVEL: " 
              << " _x_.data  = " << _x_->data
              << std::endl;
    std::cerr << "DEVEL: " 
              << " _x_.owner  = " << _x_->owner
              << std::endl;
    for (int i = 0; i < _x_->size; i++) 
      {
        std::cerr << "DEVEL: " 
                  << " _x_.data[" << i << "]  = " << _x_->data[i]
                  << std::endl;
      }
    std::cerr << "DEVEL: " 
              << " _x_.block.size  = " << _x_->block->size
              << std::endl;
    for (int i = 0; i < _x_->block->size; i++) 
      {
        std::cerr << "DEVEL: " 
                  << " _x_.block[" << i << "]  = " << _x_->block->data[i]
                  << std::endl;
      }
    std::cerr << "DEVEL ================================= " << std::endl; 
    return;
  }

  void multidimensional_minimization::init (const std::string & name_, 
                                            multidimensional_minimization_system & ms_)
  {
    _sys_ = &ms_;
    if (! _sys_->is_lock_params ()) 
      {
        _sys_->lock_params ();
      }
    size_t n = _sys_->get_number_of_free_params ();
    _init_algorithm_ (name_);

    if (_mode_ == MODE_FDF) 
      {
        _fdf_.f      = &multidimensional_minimization::f;
        _fdf_.df     = &multidimensional_minimization::df;
        _fdf_.fdf    = &multidimensional_minimization::fdf;
        _fdf_.n      = n;
        _fdf_.params = (void *) _sys_;
        _fdfmin_     = gsl_multimin_fdfminimizer_alloc (_algo_fdf_, _fdf_.n);
      }
    
    if (_mode_ == MODE_F) 
      {
        _f_.f      = &multidimensional_minimization::f;
        _f_.n      = n;
        _f_.params = (void *) _sys_;
        _fmin_     = gsl_multimin_fminimizer_alloc (_algo_f_, _f_.n);
      }
    
    _x_ = gsl_vector_alloc (n);
    gsl_vector_set_zero (_x_);
    
    _sys_->to_double_star (_x_->data, n);

    if (g_debug) 
      {
        for (int i = 0; i < n; i++) 
          {
            double par = gsl_vector_get (_x_, i);
            std::clog << "DEBUG: multidimensional_minimization::init: par=" << par << std::endl;
          }
      }
  
    if (_mode_ == MODE_FDF) 
      {
        _fdf_step_size_ = 0.01;
        _fdf_tol_       = 1.e-4;
        gsl_multimin_fdfminimizer_set (_fdfmin_, 
                                       &_fdf_, 
                                       _x_, 
                                       _fdf_step_size_, 
                                       _fdf_tol_); 
      }

    if (_mode_ == MODE_F) 
      {
        _ss_ = gsl_vector_alloc (n);
        gsl_vector_set_zero (_ss_);
        for (int i = 0; i < _sys_->get_number_of_free_params (); i++) 
          {
            gsl_vector_set (_ss_, i, _sys_->get_param (i).get_step ());
          }
        gsl_multimin_fminimizer_set (_fmin_, &_f_, _x_, _ss_); 
      }
    return;
  }

  void multidimensional_minimization::reset ()
  {
    if (_fdfmin_ != 0) 
      {
        gsl_multimin_fdfminimizer_free (_fdfmin_);
        _fdfmin_ = 0;
      }
    if (_fmin_ != 0) 
      {
        gsl_multimin_fminimizer_free (_fmin_);
        _fmin_ = 0;
      }
    if (_x_ != 0) 
      {
        gsl_vector_free (_x_);
        _x_ = 0;
      }
    if (_ss_ != 0) 
      {
        gsl_vector_free (_ss_);
        _ss_ = 0;
      }
    _algo_fdf_ = 0;
    _algo_f_   = 0;
    _fdf_step_size_ = 0.01;
    _fdf_tol_       = 1.e-4;

    _f_.f      = 0;
    _f_.n      = 0;
    _f_.params = 0;

    _fdf_.f      = 0;
    _fdf_.df     = 0;
    _fdf_.fdf    = 0;
    _fdf_.n      = 0;
    _fdf_.params = 0;

    _mode_           = MODE_F;
    _stopping_       = STOPPING_SIZE;
    _epsabs_         = DEFAULT_EPSABS;
    _at_step_action_ = 0;
    if (_sys_ != 0) 
      {
        _sys_->unlock_params ();
        _sys_ = 0;
      }
    _n_iter_ = 0;
    _fval_   = 0.0;
    return;
  }

  void multidimensional_minimization::set_max_iter (size_t max_)
  {
    _max_iter_ = max_;
    return;
  }

  void multidimensional_minimization::set_modulo_iter (size_t modulo_)
  {
    _modulo_iter_ = modulo_;
    return;
  }

  size_t multidimensional_minimization::get_max_iter () const
  {
    return _max_iter_;
  }

  size_t multidimensional_minimization::get_modulo_iter () const
  {
    return _modulo_iter_;
  }

  size_t multidimensional_minimization::get_n_iter () const
  {
    return _n_iter_;
  }
  
  double multidimensional_minimization::get_fval () const
  {
    return _fval_;
  }
  
  // ctor:
  multidimensional_minimization::multidimensional_minimization ()
  {
    _algo_fdf_ = 0;
    _algo_f_   = 0;
    _verbose_  = DEFAULT_VERBOSE;
    _mode_     = MODE_F;
    _fdfmin_   = 0;
    _fmin_     = 0;

    _f_.f      = 0;
    _f_.n      = 0;
    _f_.params = 0;
    _fdf_.f      = 0;
    _fdf_.df     = 0;
    _fdf_.fdf    = 0;
    _fdf_.n      = 0;
    _fdf_.params = 0;

    _x_              = 0;
    _ss_             = 0;
    _fdf_step_size_  = 0.01;
    _fdf_tol_        = 1.e-4;
    _max_iter_       = DEFAULT_MAX_ITER;
    _stopping_       = STOPPING_SIZE;
    _epsabs_         = 1.0;
    _at_step_action_ = 0;

    _n_iter_         = 0;
    _modulo_iter_    = DEFAULT_MODULO_ITER;       
    return;
  }

  // dtor:
  multidimensional_minimization::~multidimensional_minimization ()
  {
    reset ();
    return;
  }

  void multidimensional_minimization::_at_step_hook (int status_, 
                                                     size_t iter_, 
                                                     double * x_, 
                                                     size_t dim_, 
                                                     double f_)
  {
    if (g_debug) std::clog << "DEBUG: multidimensional_minimization::_at_step_hook: entering..." << std::endl;
    if (_at_step_action_ != 0) 
      {
        if (g_debug) std::clog << "DEBUG: multidimensional_minimization::_at_step_hook: _at_step_action_..." << std::endl;
        (*_at_step_action_) (status_, iter_, x_, dim_, f_);
      }
    if (g_debug) std::clog << "DEBUG: multidimensional_minimization::_at_step_hook: exiting." << std::endl;
    return;
  }
       
  double multidimensional_minimization::get_epsabs () const
  {
    return _epsabs_;
  }
  
  int multidimensional_minimization::minimize (double epsabs_)
  {
    // clog << ">>> TEST 100" << endl;
    size_t iter   = 0;
    int    status = 0;
    size_t dim    = 0;
    double * x    = 0;
    _epsabs_      = epsabs_;
    if (epsabs_ < 0.0)
      { 
        _epsabs_ = DEFAULT_EPSABS;
      }
    double f;

    do {
      if (g_debug) std::cerr << std::endl << "DEBUG: multidimensional_minimization::minimize: NEW ITERATTION" << std::endl;

      iter++;

      if (_mode_ == MODE_F)  
        {
          if (g_debug) std::cerr << "DEBUG: multidimensional_minimization::minimize: MODE_F" << std::endl;
          dim = _f_.n;
          status = gsl_multimin_fminimizer_iterate (_fmin_);
          if (status != 0) 
            {
              break;
            }
          double size = gsl_multimin_fminimizer_size (_fmin_);
          if (g_debug) 
            {
              std::cerr << "DEBUG: multidimensional_minimization::minimize: " 
                        << "MODE_F size=" << size 
                        << " epsabs=" << _epsabs_ << std::endl;
            }
          status = gsl_multimin_test_size (size, _epsabs_);
          x = _fmin_->x->data;    
          f = _fmin_->fval;
        }

      if (_mode_ == MODE_FDF) 
        {
          if (g_debug) std::cerr << "DEBUG: multidimensional_minimization::minimize: MODE_FDF" << std::endl;
          dim = _fdf_.n;
          status = gsl_multimin_fdfminimizer_iterate (_fdfmin_);
          if (status != 0) 
            {
              break;
            }
          status = gsl_multimin_test_gradient (_fdfmin_->gradient, _epsabs_);
          x = _fdfmin_->x->data;          
          f = _fdfmin_->f;
        }
      _at_step_hook (status, iter, x, dim, f);
      _fval_ = f;
      if ((iter % _modulo_iter_) == 0) 
        {
          clog << "mygsl::multidimensional_minimization::minimize: Iteration #" << iter << " Fval == " << _fval_ << endl;
          _sys_->print_status (clog);
          clog << endl;
        }
      
    } while ((status == GSL_CONTINUE) && (iter < _max_iter_)); 

    _n_iter_ = iter;
    if (status == GSL_SUCCESS) 
      {
        if (g_debug) std::clog << "multidimensional_minimization::minimize: END" << std::endl;
        _sys_->from_double_star (x, dim);
      }
   
    return status;     
  }

}

// end of mygsl::multidimensional_minimization.cc
