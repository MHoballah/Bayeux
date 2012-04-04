// -*- mode: c++ ; -*- 
// test_tube_vg.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/gnuplot_draw.h>
#include <genvtx/tube_vg.h>
#include <mygsl/rng.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'tube_vg'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else 
                 { 
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
               { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }

      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);
         
      geomtools::tube c (2.0, 3.0, 4.0);

      genvtx::tube_vg::g_debug = debug;
      genvtx::tube_vg vg;
      vg.set_tube (c);

      {
        vg.set_mode (genvtx::tube_vg::MODE_SURFACE);
        int surface_mask = 0;
        //surface_mask |= geomtools::tube::FACE_OUTER_SIDE;
        surface_mask |= geomtools::tube::FACE_BOTTOM;
        //surface_mask |= geomtools::tube::FACE_TOP;
        surface_mask |= geomtools::tube::FACE_INNER_SIDE;
        vg.set_surface_mask (surface_mask);
        vg.set_skin_skip (0.10);
        vg.set_skin_thickness (0.20);
        vg.dump ();
        size_t nshoots = 5000;
        geomtools::vector_3d vertex;     
        for (int i = 0; i < nshoots; i++)
          {
            vg.shoot_vertex (random, vertex);
            geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
          }
        cout << endl << endl;
      }

      {
        vg.reset ();
        vg.set_mode (genvtx::tube_vg::MODE_BULK);
        vg.set_skin_thickness (0.10);
        vg.dump ();
        size_t nshoots = 5000;
        geomtools::vector_3d vertex;     
        for (int i = 0; i < nshoots; i++)
          {
            vg.shoot_vertex (random, vertex);
            geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
          }
        cout << endl << endl;
      }

      {
        geomtools::vector_3d pos; 
        geomtools::rotation_3d rot;
        geomtools::gnuplot_draw::draw_tube (cout, pos, rot, vg.get_tube ());
        cout << endl << endl;
      }

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_tube_vg.cxx
