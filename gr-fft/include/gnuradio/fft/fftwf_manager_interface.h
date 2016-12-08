/* -*- c++ -*- */
/*
 * Copyright 2016
 *
 */

#ifndef _FFT_FFTWF_MANAGER_INTERFACE_H_
#define _FFT_FFTWF_MANAGER_INTERFACE_H_

#include <gnuradio/fft/api.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace gr {
  namespace fftwf_manager {
    // Manager class:
    class FFT_API fftwf_manager_interface {
    public:
      fftwf_manager_interface();
      virtual ~fftwf_manager_interface();
      virtual std::string wisdom_filename() = 0;
      virtual void config_threading(int nthreads) = 0;
      virtual bool import_wisdom() = 0;
      virtual bool export_wisdom() = 0;
      virtual void start() = 0;
      virtual void stop() = 0;
    };
    
    class FFT_API fftwf_manager_basic : virtual public fftwf_manager_interface {
    public:
      fftwf_manager_basic();
      virtual ~fftwf_manager_basic();
      virtual std::string wisdom_filename();
      virtual void config_threading(int nthreads);
      virtual bool import_wisdom();
      virtual bool export_wisdom();
      
      virtual void start();
      virtual void stop();
    protected:
      std::string d_filename;
    };
        
    class FFT_API fftwf_manager_cached : virtual public fftwf_manager_basic {
    public:
      fftwf_manager_cached();
      virtual ~fftwf_manager_cached();
      virtual bool import_wisdom();
      virtual bool export_wisdom();
      
      virtual void start();
      virtual void stop();
    protected:
      bool d_imported_flag;
      boost::posix_time::ptime d_last_export;
    };

    class FFT_API fftwf_manager_cached_concurrent : virtual public fftwf_manager_cached {
    public:
      fftwf_manager_cached_concurrent();
      virtual ~fftwf_manager_cached_concurrent();
      virtual bool import_wisdom();
      virtual bool export_wisdom();
      
      virtual void start();
      virtual void stop();
    };
  } /* namespace fftwf_manager */
} /*namespace gr */

#endif /* _FFT_FFTWF_MANAGER_INTERFACE_H_ */
