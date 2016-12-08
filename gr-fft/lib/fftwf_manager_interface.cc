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

#include <gnuradio/fft/fftwf_manager_interface.h>
#include <gnuradio/sys_paths.h>
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
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
namespace fs = boost::filesystem;

namespace gr {
  namespace fftwf_manager {
    fftwf_manager_interface::fftwf_manager_interface() {
      
    }
    fftwf_manager_interface::~fftwf_manager_interface() {
      
    }

    ////////////////////////////////////////////////////////////////////////////
    fftwf_manager_basic::fftwf_manager_basic() :
      fftwf_manager_interface(),
      d_filename()
    {
      d_filename = wisdom_filename();
    }
    fftwf_manager_basic::~fftwf_manager_basic() {
      
    }
    std::string fftwf_manager_basic::wisdom_filename() {
      static fs::path path;
      path = fs::path(gr::appdata_path()) / ".gr_fftw_wisdom";
      return path.string();
    }
    void fftwf_manager_basic::config_threading(int nthreads) {
      static int fftw_threads_inited = 0;
#ifdef FFTW3F_THREADS
      if (fftw_threads_inited == 0) {
        fftw_threads_inited = 1;
        fftwf_init_threads();
      }
      fftwf_plan_with_nthreads(nthreads);
#endif
    }
    bool fftwf_manager_basic::import_wisdom() {
      bool success_flag(false);
      FILE *fp = fopen(d_filename.c_str(), "r");
      if (fp != 0) {
        int r = fftwf_import_wisdom_from_file(fp);
        fclose(fp);
        if (!r) {
          fprintf (stderr, "gr::fft: can't import wisdom from %s\n", d_filename.c_str());
        }
        success_flag = (r != 0);
      }
      std::cerr << "Import wisdom " << success_flag << std::endl;
      return success_flag;
    }
    bool fftwf_manager_basic::export_wisdom() {
      bool success_flag(false);
      FILE *fp = fopen(d_filename.c_str(), "w");
      if (fp != 0) {
        fftwf_export_wisdom_to_file(fp);
        fflush(fp);
        fclose(fp);
        success_flag = true;
      }
      else {
        fprintf (stderr, "fft_impl_fftw: ");
        perror (d_filename.c_str());
      }
      std::cerr << "Export wisdom " << success_flag << std::endl;
      return success_flag;
    }
    void fftwf_manager_basic::start() {
      // NOOP
    }
    void fftwf_manager_basic::stop() {
      // NOOP
    }
    ////////////////////////////////////////////////////////////////////////////
    fftwf_manager_cached::fftwf_manager_cached() :
      fftwf_manager_basic()
    {
      
    }
    fftwf_manager_cached::~fftwf_manager_cached() {
      
    }
    bool fftwf_manager_cached::import_wisdom() {
      if (!d_imported_flag) {
        d_imported_flag = fftwf_manager_basic::import_wisdom();
      }
      return d_imported_flag;
    }
    bool fftwf_manager_cached::export_wisdom() {
      boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time(); 
      boost::posix_time::time_duration dt = now - d_last_export;
      if (dt.total_milliseconds() > 100) {
        fftwf_manager_basic::export_wisdom();
        d_last_export = now;
      }
      return true;
    }
    void fftwf_manager_cached::start() {
      d_imported_flag = false;
      d_last_export = boost::posix_time::microsec_clock::universal_time();
    }
    void fftwf_manager_cached::stop() {

    }
    ////////////////////////////////////////////////////////////////////////////
    fftwf_manager_cached_concurrent::fftwf_manager_cached_concurrent() :
      fftwf_manager_cached()
    {

    }
    fftwf_manager_cached_concurrent::~fftwf_manager_cached_concurrent() {
      
    }
    bool fftwf_manager_cached_concurrent::import_wisdom() {
      if (!d_imported_flag) {
        // Get the file lock:
        boost::interprocess::file_lock flock(d_filename.c_str());
        boost::interprocess::scoped_lock<boost::interprocess::file_lock> elock(flock);
        d_imported_flag = fftwf_manager_basic::import_wisdom();
      }
      return d_imported_flag;
    }
    bool fftwf_manager_cached_concurrent::export_wisdom() {
      boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time(); 
      boost::posix_time::time_duration dt = now - d_last_export;
      if (dt.total_milliseconds() > 100) {
        // Get the file lock:
        boost::interprocess::file_lock flock(d_filename.c_str());
        boost::interprocess::scoped_lock<boost::interprocess::file_lock> elock(flock);
        fftwf_manager_basic::export_wisdom();
        d_last_export = now;
      }
      return true;
    }
    void fftwf_manager_cached_concurrent::start() {
      d_imported_flag = false;
      d_last_export = boost::posix_time::microsec_clock::universal_time();
    }
    void fftwf_manager_cached_concurrent::stop() {

    }
  } /* namespace fftw_manager_interface */
} /* namespace gr */
