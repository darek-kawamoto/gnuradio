/* -*- c++ -*- */
/*
 * Copyright 2003,2008,2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/fft/fft.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/gr_complex.h>
#include <volk/volk.h>
#include <fftw3.h>

#ifdef _MSC_VER //http://www.fftw.org/install/windows.html#DLLwisdom
static void my_fftw_write_char(char c, void *f) { fputc(c, (FILE *) f); }
#define fftw_export_wisdom_to_file(f) fftw_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwf_export_wisdom_to_file(f) fftwf_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwl_export_wisdom_to_file(f) fftwl_export_wisdom(my_fftw_write_char, (void*) (f))

static int my_fftw_read_char(void *f) { return fgetc((FILE *) f); }
#define fftw_import_wisdom_from_file(f) fftw_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwf_import_wisdom_from_file(f) fftwf_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwl_import_wisdom_from_file(f) fftwl_import_wisdom(my_fftw_read_char, (void*) (f))
#endif //_MSC_VER

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

namespace gr {
  namespace fftwf_manager {
    // Static memory setup:
    boost::mutex fftwf_manager_static_objects::s_fftwf_manager_lock;
    boost::mutex fftwf_manager_static_objects::s_fftwf_planner_lock;
    fftwf_manager_interface* fftwf_manager_static_objects::s_fftwf_manager = NULL;
    namespace {
      int x = fftwf_manager_static_objects::setup_static_memory();
    }
    int fftwf_manager_static_objects::setup_static_memory() {
      s_fftwf_manager = new fftwf_manager_basic;
      s_fftwf_manager->start();
      std::atexit(fftwf_manager_static_objects::teardown_static_memory);
      return 1;
    }
    void fftwf_manager_static_objects::teardown_static_memory() {
      s_fftwf_manager->export_wisdom();
      s_fftwf_manager->stop();
      delete s_fftwf_manager;
      s_fftwf_manager = NULL;
    }
    
    boost::mutex& mutex() {
      return fftwf_manager_static_objects::s_fftwf_planner_lock;
    }

    std::string wisdom_filename() {
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      return get_fftwf_manager()->wisdom_filename();
    }
    
    void config_threading(int nthreads) {
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      return get_fftwf_manager()->config_threading(nthreads);
    }
    
    bool import_wisdom() {
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      return get_fftwf_manager()->import_wisdom();
    }

    bool export_wisdom() {
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      return get_fftwf_manager()->export_wisdom();
    }
    void start_manager() {
      boost::mutex::scoped_lock planner_lock(get_fftwf_planner_lock());
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      return get_fftwf_manager()->start();
    }
    void stop_manager() {
      boost::mutex::scoped_lock planner_lock(get_fftwf_planner_lock());
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      return get_fftwf_manager()->stop();
    }
    
    // fftwf_manager 
    boost::mutex& get_fftwf_manager_lock() {
      return fftwf_manager_static_objects::s_fftwf_manager_lock;
    }
    boost::mutex& get_fftwf_planner_lock() {
      return fftwf_manager_static_objects::s_fftwf_planner_lock;
    }
    fftwf_manager_interface* get_fftwf_manager() {
      return fftwf_manager_static_objects::s_fftwf_manager;
    }
    void set_fftwf_manager(fftwf_manager_interface* manager) {
      boost::mutex::scoped_lock lock(get_fftwf_manager_lock());
      fftwf_manager_static_objects::s_fftwf_manager->stop();
      delete fftwf_manager_static_objects::s_fftwf_manager;
      fftwf_manager_static_objects::s_fftwf_manager = manager;
      fftwf_manager_static_objects::s_fftwf_manager->start();
    }
  } /* namespace fftw_manager */
} /* namespace gr */
