// -*-mode: c++; -*-
// mygsl::fft_real.h

#ifndef __mygsl__fft_real_h 
#define __mygsl__fft_real_h 1

#include <iostream>
#include <vector>

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

namespace mygsl {

  class fft_real
  {
  private:

    bool   _initialized_;
    size_t _n_;
    double _start_;
    double _stop_;
    double _step_;       // grid interval/scanning period
    double _F0_;         // scanning frequency
    double * _data_;     // sampled data
    double * _data_rec_; 
    double * _freq_;
    double * _tmp_;
    double _Nyquist_freq_;
    double _min_freq_cutoff_;
    double _max_freq_cutoff_;

    // gsl stuff:
    gsl_fft_real_wavetable *        _the_real_;
    gsl_fft_halfcomplex_wavetable * _hc_;
    gsl_fft_real_workspace *        _work_;

  public:

    fft_real ();

    ~fft_real ();

    void reset ();

    void init (const std::vector<double> & sample_ , 
	       double start_ , 
	       double step_ ,
	       double min_freq_cutoff_ ,
	       double max_freq_cutoff_);

    void init (const std::vector<double> & sample_ , 
	       double start_ , 
	       double step_ ,
	       double max_freq_cutoff_);

    double get_frequency_step () const;

    void dump (std::ostream & out_ , bool dump_arrays_ = true) const;

    void compute_fourier_spectrum (std::vector<double> & freq_spectrum_) const;

    void compute_filtered_data (std::vector<double> & filtered_data_) const;

  private:

    void _process_filter_ ();

  public:

    void process ();

    static int main (int argc_ , char ** argv_);

  };

}

#endif //  __mygsl__fft_real_h

// end of mygsl::fft_real.h
