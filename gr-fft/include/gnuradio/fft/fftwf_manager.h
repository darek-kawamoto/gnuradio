/* -*- c++ -*- */
/*
 * Copyright 2016
 *
 */

#ifndef _FFT_FFTWF_MANAGER_H_
#define _FFT_FFTWF_MANAGER_H_

#include <gnuradio/fft/api.h>
#include <gnuradio/gr_complex.h>
#include <boost/thread.hpp>

// The following #define helps bridge the gap between this branch and the old
// gr-fft.  Any code which wants to use features from the new branch but
// be also be able to build against the old branch can use this #define flag
// to detect presence of the new fftwf_manager namespace.
#define GR_FFT_HAVE_FFTWF_MANAGER 1

namespace gr {
  namespace fftwf_manager {
    /*!
     * \brief Export reference to planner mutex for those apps that
     * want to use FFTW w/o using the fft_impl_fftw* classes.
     */
    class FFT_API planner {
    public:
      typedef boost::mutex::scoped_lock scoped_lock;
      /*!
       * Return reference to planner mutex
       */
      static boost::mutex &mutex();
    };
    std::string wisdom_filename();
    void import_wisdom();
    void config_threading(int nthreads);
    void export_wisdom();
    
  } /* namespace fftwf_manager */
} /*namespace gr */

#endif /* _FFT_FFTWF_MANAGER_H */
