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
    boost::mutex& planner::mutex() {
      static boost::mutex  s_planning_mutex;
      return s_planning_mutex;
    }

    std::string wisdom_filename() {
      static fs::path path;
      path = fs::path(gr::appdata_path()) / ".gr_fftw_wisdom";
      return path.string();
    }

    void import_wisdom() {
      const std::string filename = wisdom_filename ();
      FILE *fp = fopen(filename.c_str(), "r");
      if (fp != 0) {
        int r = fftwf_import_wisdom_from_file(fp);
        fclose(fp);
        if (!r) {
          fprintf (stderr, "gr::fft: can't import wisdom from %s\n", filename.c_str());
        }
      }
    }

    void config_threading(int nthreads) {
      static int fftw_threads_inited = 0;
#ifdef FFTW3F_THREADS
      if (fftw_threads_inited == 0) {
        fftw_threads_inited = 1;
        fftwf_init_threads();
      }
      fftwf_plan_with_nthreads(nthreads);
#endif
    }

    void export_wisdom() {
      const std::string filename = wisdom_filename ();
      FILE *fp = fopen(filename.c_str(), "w");
      if (fp != 0) {
        fftwf_export_wisdom_to_file(fp);
        fclose(fp);
      }
      else {
        fprintf (stderr, "fft_impl_fftw: ");
        perror (filename.c_str());
      }
    }
    
  } /* namespace fftw_manager */
} /* namespace gr */
