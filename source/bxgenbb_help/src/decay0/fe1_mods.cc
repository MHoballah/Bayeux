
#include <cmath>

#include <gsl/gsl_math.h>

#include <genbb_help/decay0/fe1_mods.h>
#include <genbb_help/decay0/bb.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/fermi.h>

namespace genbb {
  namespace decay0 {

    double decay0_fe1_mod1(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      if (e1 > e0) return 0.0;
      double e2 = e0 - e1;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)*
        (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2);
    }

    double decay0_fe1_mod2(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      if (e1 > e0) return 0.0;
      double e2 = e0 - e1;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)*
        (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)*
        gsl_pow_2(e0-2.*e1);
    }

    double decay0_fe1_mod3(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      if (e1 > e0) return 0.0;
      double e2 = e0 - e1;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return p1*decay0::decay0_fermi(Zdbb,e1)*p2*decay0::decay0_fermi(Zdbb,e2)*
        (2. * gsl_pow_2(p1*p2) + 9. * ((e1+emass)*(e2+emass)+gsl_pow_2(emass))*
         (gsl_pow_2(p1)+gsl_pow_2(p2)));
    }
  
    double decay0_fe1_mod7(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      if (e1 > e0) return 0.0;
      double e2 = e0 - e1;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return p1*decay0::decay0_fermi(Zdbb,e1)*p2*decay0::decay0_fermi(Zdbb,e2)*
        ((e1+emass)*(e2+emass)+gsl_pow_2(emass))*(gsl_pow_2(p1)+gsl_pow_2(p2));
    }

    double decay0_fe1_mod10(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      if (e1 > e0) return 0.0;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      return (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)*gsl_pow_5(e0-e1);
    }

    double decay0_fe1_mod17(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & e0   = pars->e0;
      if (e1 > e0) return 0.0;
      double e2 = e0 - e1;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      return (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)*
        (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2);
    }

    double decay0_fe1_mod18(double e1, void * params)
    {
      static double emass = decay0_emass();
      const bbpars * pars = static_cast<const bbpars *>(params);
      const double & Zdbb = pars->Zd;
      const double & Adbb = pars->Ad;
      const double & e0   = pars->e0;
      const double & chi_GTw  = pars->chi_GTw;
      const double & chi_Fw   = pars->chi_Fw;
      const double & chip_GT  = pars->chip_GT;
      const double & chip_F   = pars->chip_F;
      const double & chip_T   = pars->chip_T;
      const double & chip_P   = pars->chip_P;
      const double & chip_R   = pars->chip_R;
      if (e1 > e0) return 0.0;
      double e2 = e0 - e1;
      double p1 = std::sqrt(e1*(e1+2.*emass));
      double p2 = std::sqrt(e2*(e2+2.*emass));
      // total energies in the units of electron mass
      double et0=e0/emass+1.;
      double et1=e1/emass+1.;
      double et2=e2/emass+1.;
      double a1=(et1*et2-1.)*gsl_pow_2(et1-et2)/(2.*et1*et2);
      double a2=-2.*gsl_pow_2(et1-et2)/(9.*et1*et2);
      double a3=2.*(et1*et2-1.)/(81.*et1*et2);
      double r=3.107526e-3*std::pow(Adbb,(1./3.));
      double rksi=3./137.036*Zdbb+r*et0;
      double a4=8.*(et1*et2+1.)/(r*r*et1*et2);
      double a5=-8.*(rksi*(et1*et2+1.)-2.*r*et0)/(3.*r*r*et1*et2);
      double a6=2.*((rksi*rksi+4.*r*r)*(et1*et2+1.)-4.*rksi*r*et0)/
        (9.*r*r*et1*et2);
      //double chi_1plus =chip_GT+3.*chip_F-6.*chip_T;
      double chi_1minus=chip_GT-3.*chip_F-6.*chip_T;
      double chi_2plus =chi_GTw+chi_Fw-chi_1minus/9.;
      double a_eta=a1*gsl_pow_2(chi_2plus)
        +a2*chi_2plus*chi_1minus
        +a3*gsl_pow_2(chi_1minus)
        +a4*gsl_pow_2(chip_R)
        +a5*chip_R*chip_P
        +a6*gsl_pow_2(chip_P);
      return (e1+emass)*p1*decay0::decay0_fermi(Zdbb,e1)*
        (e2+emass)*p2*decay0::decay0_fermi(Zdbb,e2)*a_eta;
    }
    
  } // end of namespace decay0 
} // end of namespace genbb 
