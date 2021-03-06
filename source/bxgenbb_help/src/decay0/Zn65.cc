
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Zn65.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/alpha.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/positron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/pair.h>
#include <genbb_help/decay0/nucltransK.h>
#include <genbb_help/decay0/nucltransKL.h>
#include <genbb_help/decay0/nucltransKLM.h>
#include <genbb_help/decay0/nucltransKLM_Pb.h>
#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/beta_1fu.h>
#include <genbb_help/decay0/PbAtShell.h>

namespace genbb {
  namespace decay0 {

    // Zn65.f 
    // This file was extracted from the 'decay0' program by V.I. Tretyak
    // Copyright 1995-2011 V.I. Tretyak
    // This program is free software
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation
    // your option) any later version.
    // 
    // This program is distributed in the hope that it will be useful, but
    // WITHOUT ANY WARRANTY
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    // General Public License for more details.
    // 
    // You should have received a copy of the GNU General Public License
    // along with this program
    // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    // 

    void Zn65(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double pbec;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Zn65 decay (NDS 69(1993)209 and NNDC online corrections on 28.03.2007).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 28.03.2007.
      thnuc=2.1086784E+07;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pbec=100.*prng_();
      if (pbec <= 1.42) goto label_1;
      goto label_2;
      // beta+ decay to g.s. of 65-Cu
    label_1  :
      decay0_pair(prng_, event_, 0.329,0.,0.,tdlev);
      return;
      // X ray after EC to 65-Cu
    label_2  :
      decay0_gamma(prng_, event_, 0.009,0.,0.,t);
      if (pbec <= 52.03) goto label_11160;
      goto label_10000;
    label_11160  :
      thlev=0.285e-12;
      p=100.*prng_();
      if (p <= 99.994) goto label_11161;
      goto label_11162;
    label_11161  :
      decay0_nucltransK(prng_, event_, 1.116,0.009,1.9e-4,1.0e-6,tclev,thlev,tdlev);
      return;
    label_11162  :
      decay0_nucltransK(prng_, event_, 0.345,0.009,6.8e-3,0.,tclev,thlev,tdlev);
      goto label_77100;
    label_77100  :
      thlev=99.e-15;
      decay0_nucltransK(prng_, event_, 0.771,0.009,3.8e-4,0.,tclev,thlev,tdlev);
    label_10000  :
      return;
    }
    // end of Zn65.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Zn65.cc
// Local Variables: --
// mode: c++ --
// End: --
