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
#include <gnuradio/fft/fftwf_manager_interface.h>

// The following #define helps bridge the gap between this branch and the old
// gr-fft.  Any code which wants to use features from the new branch but
// be also be able to build against the old branch can use this #define flag
// to detect presence of the new fftwf_manager namespace.
#define GR_FFT_HAVE_FFTWF_MANAGER

namespace gr {
  namespace fftwf_manager {
    FFT_API boost::mutex &mutex();
    
    // Wisdom accessor functions:
    FFT_API std::string wisdom_filename();
    FFT_API void config_threading(int nthreads);
    FFT_API bool import_wisdom();
    FFT_API bool export_wisdom();
    FFT_API void start_manager();
    FFT_API void stop_manager();
    
    FFT_API boost::mutex& get_fftwf_manager_lock();
    FFT_API boost::mutex& get_fftwf_planner_lock();
    FFT_API fftwf_manager_interface* get_fftwf_manager();
    FFT_API void set_fftwf_manager(fftwf_manager_interface*);
      
    // Manager management:
    class FFT_API fftwf_manager_static_objects {
    public:
      static int setup_static_memory();
      static void teardown_static_memory();
      static boost::mutex s_fftwf_manager_lock;
      static boost::mutex s_fftwf_planner_lock;
      static fftwf_manager_interface* s_fftwf_manager;
    };
    
    
    
  } /* namespace fftwf_manager */
} /*namespace gr */

#endif /* _FFT_FFTWF_MANAGER_H */
