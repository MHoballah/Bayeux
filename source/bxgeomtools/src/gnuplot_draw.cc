// -*- mode: c++; -*- 
/* gnuplot_draw.cc
 */

#include <geomtools/gnuplot_draw.h>
#include <cmath>
#include <stdexcept>

#include <geomtools/i_placement.h>
#include <geomtools/i_object_3d.h>

#include <geomtools/placement.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/sphere.h>
#include <geomtools/tube.h>
#include <geomtools/line_3d.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/rectangle.h>
#include <geomtools/circle.h>
#include <geomtools/disk.h>
#include <geomtools/regular_polygon.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>
#include <geomtools/tessellation.h>
#include <geomtools/union_3d.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>

#include <geomtools/color.h>


namespace geomtools {

  bool   gnuplot_draw::g_using_color   = false;
  double gnuplot_draw::g_current_color = 1.0;

  void 
  gnuplot_draw::basic_draw_point (std::ostream & out_, 
				  double x_, double y_, double z_)
  {
    basic_draw_point (out_, x_, y_, z_, true);
    return;
  }

  void 
  gnuplot_draw::basic_draw_point (std::ostream & out_, 
				 const vector_3d & v_)
  {
    basic_draw_point (out_, v_, true);
    return;
  }

  void 
  gnuplot_draw::basic_draw_point_with_color (std::ostream & out_, 
					     double x_, 
					     double y_, 
					     double z_, 
					     double color_,
					     bool   endl_)
  {
    double color = color_;
    out_.precision (15);
    out_ << x_ << ' ' 
	 << y_ << ' ' 
	 << z_ << ' ' 
	 << color;
    if (endl_) out_ << std::endl;
    return;
  }

  void 
  gnuplot_draw::basic_draw_point_with_color (std::ostream & out_, 
					     const vector_3d & point_,
					     double color_,
					     bool   endl_)
  {
    basic_draw_point_with_color (out_,
				 point_.x (),
				 point_.y (),
				 point_.z (),
				 color_,
				 endl_);
    return;
  }

  void 
  gnuplot_draw::basic_draw_point (std::ostream & out_, 
				  double x_, double y_, double z_,
				  bool endl_)
  { 
    if (gnuplot_draw::g_using_color)
      {
	basic_draw_point_with_color (out_, 
				     x_, y_, z_, 
				     gnuplot_draw::g_current_color, false);
      }
    else
      {
	out_.precision (15);
	out_ << x_ << ' ' 
	     << y_ << ' ' 
	     << z_;
      }
    if (endl_) out_ << std::endl;
    return;
  }

  void 
  gnuplot_draw::basic_draw_point (std::ostream & out_, 
				  const vector_3d & point_,
				  bool endl_)
  {
    gnuplot_draw::basic_draw_point (out_, 
				    point_.x (), 
				    point_.y (),
				    point_.z (), 
				    endl_);
    return;
  }

  void 
  gnuplot_draw::basic_draw_facet3 (std::ostream & out_, 
				   const vector_3d & p1_, 
				   const vector_3d & p2_, 
				   const vector_3d & p3_,
				   double color_)
  {
    basic_draw_point_with_color (out_, p1_, color_, true);
    basic_draw_point_with_color (out_, p2_, color_, true);
    out_ << endl;
    basic_draw_point_with_color (out_, p3_, color_, true);
    basic_draw_point_with_color (out_, p3_, color_, true);
    out_ << endl;
    out_ << endl;
    return;
  }

   void 
    gnuplot_draw::basic_draw_facet3 (std::ostream & out_, 
		       const vector_3d & p1_, 
		       const vector_3d & p2_, 
		       const vector_3d & p3_, 
		       const vector_3d & p4_,
		       double color_)
    {
      basic_draw_point_with_color (out_, p1_, color_);
      basic_draw_point_with_color (out_, p2_, color_);
      out_ << endl;
      basic_draw_point_with_color (out_, p4_, color_);
      basic_draw_point_with_color (out_, p3_, color_);
      out_ << endl;
      out_ << endl;
      return;
    }

  void 
  gnuplot_draw::basic_draw_polyline (std::ostream & out_, 
				     const polyline_t & pl_)
  {
    for (polyline_t::const_iterator i = pl_.begin ();
	 i != pl_.end ();
	 ++i) 
      {
	basic_draw_point (out_, *i);
      }
    out_ << std::endl;
    return;
  }

  void 
  gnuplot_draw::draw_line (std::ostream & out_, 
			   const line_3d & l_)
  {
    draw_line (out_, l_.get_first (), l_.get_last ());
    return;
  }

  void 
  gnuplot_draw::draw_line (std::ostream & out_, 
			   const vector_3d & start_ , 
			   const vector_3d & stop_)
  {
    basic_draw_point (out_, start_);
    basic_draw_point (out_, stop_);
    out_ << std::endl;
    return;
  }

  void 
  gnuplot_draw::draw_polyline (std::ostream & out_, 
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       const polyline_3d & p_, 
			       bool  more_)
  {
    polyline_3d::point_col c;
    p_.make_vertex_collection (c);
    draw_polyline (out_, position_, rotation_, c, more_);
    return;
  }

  void 
  gnuplot_draw::draw_polyline (std::ostream & out_, 
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       const polyline_t & pl_, 
			       bool  more_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();
    polyline_t polyline;
    vector_3d first;
    bool start = true;
    for (polyline_t::const_iterator i = pl_.begin ();
	 i != pl_.end ();
	 i++) 
      {
	vector_3d P (*i);
	P.transform (inverse_rotation);
	P += position_;
	if (start) 
	  {
	    first = P;
	    start = false;
	  }
	polyline.push_back (P);	
      } 
    if (more_) polyline.push_back (first);
    basic_draw_polyline (out_, polyline);
    return;
  }

  void 
  gnuplot_draw::draw_segment (std::ostream & out_, 
			      const vector_3d & position_, 
			      const rotation_3d & rotation_,  
			      const vector_3d & start_, 
			      const vector_3d & stop_)
  {
    static bool even = true;
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();
    vector_3d A (start_);
    A.transform (inverse_rotation);
    A += position_;
    vector_3d B (stop_);
    B.transform (inverse_rotation);
    B += position_;
    polyline_t polyline;
    polyline.push_back (A);
    if (even)
      {
	vector_3d M = 0.5 * (A + B);
	polyline.push_back (M);
	even = false;
      }
    else
      {
	even = true;
      }
    polyline.push_back (B);
    basic_draw_polyline (out_, polyline);
    return;
  }

  void 
  gnuplot_draw::draw_segment (std::ostream & out_, 
			      const vector_3d & position_, 
			      const rotation_3d & rotation_,  
			      const line_3d & l_)
  {
    draw_segment (out_, position_, rotation_, 
		  l_.get_first (),
		  l_.get_last ());
    return;
  }

  void 
  gnuplot_draw::draw_line (std::ostream & out_, 
			   const vector_3d & position_, 
			   const rotation_3d & rotation_,  
			   const line_3d & l_)
  {
    draw_segment (out_, position_, rotation_, l_);
  }
  
  void 
  gnuplot_draw::draw_rectangle (std::ostream & out_, 
				const vector_3d & position_, 
				const rotation_3d & rotation_, 
				double length_, 
				double width_, 
				bool more_)
  {
    vector_3d A ( 0.5 * length_,  0.5 * width_, 0.);
    vector_3d B ( 0.5 * length_, -0.5 * width_, 0.);
    vector_3d C (-0.5 * length_, -0.5 * width_, 0.);
    vector_3d D (-0.5 * length_,  0.5 * width_, 0.);

    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    vector_3d A2 (A);
    A2.transform (inverse_rotation);
    A2 += position_;

    vector_3d B2 (B);
    B2.transform (inverse_rotation);
    B2 += position_;

    vector_3d C2 (C);
    C2.transform (inverse_rotation);
    C2 += position_;

    vector_3d D2 (D);
    D2.transform (inverse_rotation);
    D2 += position_;

    polyline_t polyline;
    polyline.push_back (A2);
    polyline.push_back (B2);
    polyline.push_back (C2);
    polyline.push_back (D2);
    polyline.push_back (A2);
    if (more_) polyline.push_back (B2); 
    basic_draw_polyline (out_,polyline);
    return;
  }
  
  void 
  gnuplot_draw::draw_rectangle (std::ostream & out_, 
				const vector_3d & position_, 
				const rotation_3d & rotation_, 
				const rectangle & r_, 
				bool more_)
  {
    draw_rectangle (out_, position_, rotation_, 
		    r_.get_x (), r_.get_y (), more_);
    return;
  }

  void 
  gnuplot_draw::draw_box (std::ostream & out_, 
			  const vector_3d & pos_, 
			  const rotation_3d & rot_,
			  const box & b_)
  {
    draw_box (out_, pos_, rot_, 
	      b_.get_x (),
	      b_.get_y (),
	      b_.get_z ());
    return;
  }
  
  void 
  gnuplot_draw::draw_box (std::ostream & out_,
			  const vector_3d & position_, 
			  const rotation_3d & rotation_,
			  double length_, 
			  double width_, 
			  double height_)
  {
    vector_3d A ( 0.5 * length_,  0.5 * width_,  0.5 * height_);
    vector_3d B ( 0.5 * length_, -0.5 * width_,  0.5 * height_);
    vector_3d C (-0.5 * length_, -0.5 * width_,  0.5 * height_);
    vector_3d D (-0.5 * length_,  0.5 * width_,  0.5 * height_);
    vector_3d P ( 0.5 * length_,  0.5 * width_, -0.5 * height_);
    vector_3d Q ( 0.5 * length_, -0.5 * width_, -0.5 * height_);
    vector_3d R (-0.5 * length_, -0.5 * width_, -0.5 * height_);
    vector_3d S (-0.5 * length_,  0.5 * width_, -0.5 * height_);

    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    vector_3d A2 (A);
    A2.transform (inverse_rotation);
    A2 += position_;

    vector_3d B2 (B);
    B2.transform (inverse_rotation);
    B2 += position_;

    vector_3d C2 (C);
    C2.transform (inverse_rotation);
    C2 += position_;

    vector_3d D2 (D);
    D2.transform (inverse_rotation);
    D2 += position_;

    vector_3d P2 (P);
    P2.transform (inverse_rotation);
    P2 += position_;

    vector_3d Q2 (Q);
    Q2.transform (inverse_rotation);
    Q2 += position_;

    vector_3d R2 (R);
    R2.transform (inverse_rotation);
    R2 += position_;

    vector_3d S2 (S);
    S2.transform (inverse_rotation);
    S2 += position_;

    polyline_t polyline;
    polyline.push_back (A2);
    polyline.push_back (B2);
    polyline.push_back (C2);
    polyline.push_back (D2);
    polyline.push_back (A2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (P2);
    polyline.push_back (Q2);
    polyline.push_back (R2);
    polyline.push_back (S2);
    polyline.push_back (P2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (A2);
    polyline.push_back (P2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (B2);
    polyline.push_back (Q2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (C2);
    polyline.push_back (R2);
    basic_draw_polyline (out_, polyline);

    polyline.clear ();
    polyline.push_back (D2);
    polyline.push_back (S2);
    basic_draw_polyline (out_, polyline);

    return;
  }

  void 
  gnuplot_draw::draw_cylinder (std::ostream & out_, 
			       const vector_3d & pos_, 
			       const rotation_3d & rot_,
			       const cylinder & c_, 
			       size_t arc_sampling_)
  {
    draw_cylinder (out_, pos_, rot_, 
		   c_.get_r (),
		   c_.get_z (),
		   arc_sampling_);
    return;
  }
  

  void 
  gnuplot_draw::draw_cylinder (std::ostream & out_,
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       double radius_, 
			       double height_,
			       size_t arc_sampling_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t sample =  arc_sampling_;
    double dangle =  2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline_top;
    polyline_t polyline_bottom;
    polyline_t polyline_segment;
    for (size_t i = 0; i <= sample ; ++i) 
      {
	vector_3d P,Q;
	double angle = i * dangle;
	P.set (radius_ * std::cos (angle), 
	       radius_ * std::sin (angle),  
	       0.5 * height_);
	Q.set (radius_ * std::cos (angle), 
	       radius_ * std::sin (angle), 
	       -0.5 * height_);
	vector_3d P2 (P);
	P2.transform (inverse_rotation);
	P2 += position_;
	polyline_top.push_back (P2);
	vector_3d Q2 (Q);
	Q2.transform (inverse_rotation);
	Q2 += position_;
	polyline_bottom.push_back (Q2);
	polyline_segment.clear ();
	polyline_segment.push_back (P2);
	polyline_segment.push_back (Q2);
	basic_draw_polyline (out_, polyline_segment);
      }
    basic_draw_polyline (out_, polyline_top);
    basic_draw_polyline (out_, polyline_bottom);
    
    return;
  }

  void 
  gnuplot_draw::draw_tube (std::ostream & out_,
			   const vector_3d & position_, 
			   const rotation_3d & rotation_,
			   const tube & t_,
			   size_t arc_sampling_)
  {
    draw_tube (out_, position_, rotation_, 
	       t_.get_inner_r (),
	       t_.get_outer_r (),
	       t_.get_z (), arc_sampling_);
  }

  void 
  gnuplot_draw::draw_tube (std::ostream & out_,
			   const vector_3d & position_, 
			   const rotation_3d & rotation_,
			   double inner_radius_, 
			   double outer_radius_, 
			   double height_,
			   size_t arc_sampling_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t sample = arc_sampling_;
    double dangle = 2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline_top_i;
    polyline_t polyline_bottom_i;
    polyline_t polyline_segment_i;
    polyline_t polyline_top_o;
    polyline_t polyline_bottom_o;
    polyline_t polyline_segment_o;
    polyline_t polyline_endcap_top;
    polyline_t polyline_endcap_bottom;
    for (size_t i = 0; i <= sample ; ++i) 
      {
	vector_3d P_i, Q_i;
	vector_3d P_o, Q_o;
	double angle = i * dangle;
	P_i.set (inner_radius_ * std::cos (angle),
		 inner_radius_ * std::sin (angle),
		 0.5 * height_);
	Q_i.set (inner_radius_ * std::cos (angle),
		 inner_radius_ * std::sin (angle),
		 -0.5 * height_);
	P_o.set (outer_radius_ * std::cos (angle),
		 outer_radius_ * std::sin (angle),
		 0.5 * height_);
	Q_o.set (outer_radius_ * std::cos (angle),
		 outer_radius_ * std::sin (angle),
		 -0.5 * height_);
	vector_3d P2_i (P_i);
	P2_i.transform (inverse_rotation);
	P2_i += position_;
	polyline_top_i.push_back (P2_i);
	vector_3d Q2_i (Q_i);
	Q2_i.transform (inverse_rotation);
	Q2_i += position_;

	vector_3d P2_o (P_o);
	P2_o.transform (inverse_rotation);
	P2_o += position_;
	polyline_top_o.push_back (P2_o);
	vector_3d Q2_o (Q_o);
	Q2_o.transform (inverse_rotation);
	Q2_o += position_;

	polyline_bottom_i.push_back (Q2_i);
	polyline_segment_i.clear ();
	polyline_segment_i.push_back (P2_i);
	polyline_segment_i.push_back (Q2_i);
	basic_draw_polyline (out_, polyline_segment_i);

	polyline_bottom_o.push_back (Q2_o);
	polyline_segment_o.clear ();
	polyline_segment_o.push_back (P2_o);
	polyline_segment_o.push_back (Q2_o);
	basic_draw_polyline (out_, polyline_segment_o);

	polyline_endcap_top.clear ();
	polyline_endcap_top.push_back (P2_i);
	polyline_endcap_top.push_back (P2_o);
	basic_draw_polyline (out_, polyline_endcap_top);

	polyline_endcap_bottom.clear ();
	polyline_endcap_bottom.push_back (Q2_i);
	polyline_endcap_bottom.push_back (Q2_o);
	basic_draw_polyline (out_, polyline_endcap_bottom);

      }
    basic_draw_polyline (out_, polyline_top_i);
    basic_draw_polyline (out_, polyline_bottom_i);
    basic_draw_polyline (out_, polyline_top_o);
    basic_draw_polyline (out_, polyline_bottom_o);
    
    return;
  }

  void 
  gnuplot_draw::draw_circle (std::ostream & out_, 
			     const vector_3d & position_, 
			     const rotation_3d & rotation_, 
			     const circle & c_,
			     size_t arc_sampling_)
  {
    draw_circle (out_, position_, rotation_, c_.get_r (), arc_sampling_);
    return;
  }

  void 
  gnuplot_draw::draw_circle (std::ostream & out_, 
			     const vector_3d & position_, 
			     const rotation_3d & rotation_, 
			     double radius_,
			     size_t arc_sampling_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t sample =  arc_sampling_;
    double dangle = 2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline;
    for (size_t i = 0; i <= sample; ++i) 
      {
	vector_3d P;
	double angle = i * dangle;
	P.set (radius_ * std::cos (angle),
	       radius_ * std::sin (angle),
	       0.0);
	vector_3d P2 (P);
	P2.transform (inverse_rotation);
	P2 += position_;
	polyline.push_back (P2);
      }
    basic_draw_polyline (out_, polyline);
    return;
  }


  void gnuplot_draw::draw_disk (std::ostream & out_, 
				const vector_3d & position_, 
				const rotation_3d & rotation_, 
				const disk & d_,
				size_t arc_sampling_)
  {
    draw_disk (out_, position_, rotation_, d_.get_r (), arc_sampling_);
    return;
  }

  void 
  gnuplot_draw::draw_disk (std::ostream & out_, 
			   const vector_3d & position_, 
			   const rotation_3d & rotation_, 
			   double radius_,
			   size_t arc_sampling_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t sample =  arc_sampling_;
    double dangle = 2 * M_PI * CLHEP::radian / sample;
    polyline_t polyline;
    for (size_t i = 0; i <= sample; ++i) 
      {
	vector_3d O;
	vector_3d P;
	double angle = i * dangle;
	P.set (radius_ * std::cos (angle),
	       radius_ * std::sin (angle),
	       0.0);
	vector_3d P2 (P);
	P2.transform (inverse_rotation);
	P2 += position_;
	polyline.push_back (P2);
	draw_segment (out_, position_, rotation_,
		      O, P);
      }
    basic_draw_polyline (out_, polyline);
    return;
  }

  void 
  gnuplot_draw::draw_regular_polygon (std::ostream & out_, 
				      const vector_3d & position_, 
				      const rotation_3d & rotation_, 
				      const regular_polygon & rp_,
				      bool draw_radius_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    polyline_t polyline;
    for (size_t i = 0; i <= rp_.get_n_sides(); ++i) 
      {
	vector_3d O;
	vector_3d P;
	rp_.get_vertex (i, P);
	vector_3d P2 (P);
	P2.transform (inverse_rotation);
	P2 += position_;
	polyline.push_back (P2);
	if (draw_radius_)
	  {
	    draw_segment (out_, position_, rotation_,
			  O, P);
	  }
      }
    basic_draw_polyline (out_, polyline);
    return;
  }

  void 
  gnuplot_draw::draw_sphere (std::ostream & out_, 
			     const vector_3d & position_, 
			     const rotation_3d & rotation_, 
			     const sphere & s_,
			     size_t arc_sampling_,
			     size_t z_sampling_)
  {
    draw_sphere (out_, position_, rotation_, s_.get_r (), 
		 arc_sampling_, z_sampling_);
  }

  void 
  gnuplot_draw::draw_sphere (std::ostream & out_,
			     const vector_3d & position_, 
			     const rotation_3d & rotation_,
			     double radius_, 
			     size_t arc_sampling_,
			     size_t z_sampling_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t phy_sample = arc_sampling_;
    size_t z_sample = z_sampling_;
    double dphi =  2 * M_PI * CLHEP::radian / phy_sample;
    double dz   =  2 * radius_ / z_sample;
    double factor = 0.25;

    // draw meridians:
    {
      for (size_t i = 0; i <= phy_sample ; ++i) 
	{
	  polyline_t polyline_meridian;
	  double phi = i * dphi;
	  double z = -radius_;
	  for (int j = 0; j <= (int) z_sample + 2 + (i == 0 ? 1: 0); j++)
	    {
	      //double z = -radius_ + j * dz;
	      double theta = std::acos (z / radius_);
	      vector_3d P;
	      P.set (radius_ * std::cos (phi) * std::sin (theta), 
		     radius_ * std::sin (phi) * std::sin (theta),  
		     z);
	      vector_3d P2 (P);
	      P2.transform (inverse_rotation);
	      P2 += position_;
	      polyline_meridian.push_back (P2);
	      // increment z:
	      if (j == 0) z += factor * dz;
	      else if (j == 1) z += (1 - factor) * dz;
	      else if (j == z_sample) z += (1 - factor) * dz;
	      else if (j == z_sample + 1) z += factor * dz;
	      else if (j == z_sample + 2) z = radius_;
	      else if (j == z_sample + 3) z = radius_ - 0.5 * factor;
	      else z += dz;
	    }
	  basic_draw_polyline (out_, polyline_meridian);
	}
    }

    // draw parallels:
    {
      double z = -radius_ + factor * dz;
      for (int j = 1; j <= (int) z_sample + 1; j++)
	{
	  polyline_t polyline_parallel;
	  for (size_t i = 0; i <= phy_sample ; ++i) 
	    {
	      vector_3d P;
	      double phi = i * dphi;
	      double theta = std::acos (z / radius_);
	      P.set (radius_ * std::cos (phi) * std::sin (theta), 
		     radius_ * std::sin (phi) * std::sin (theta),  
		     z);
	      vector_3d P2 (P);
	      P2.transform (inverse_rotation);
	      P2 += position_;
	      polyline_parallel.push_back (P2);
	    }
	  basic_draw_polyline (out_, polyline_parallel);
	  // increment z:
	  if (j == 1) z += (1 - factor) * dz;
	  else if (j == z_sample) z += (1 - factor) * dz;
	  else if (j == z_sample + 1) z += factor * dz;
	  else if (j == z_sample + 2) z = radius_;
	  else if (j == z_sample + 3) z = radius_ - 0.5 * factor;
	  else z += dz;
	}
    }
    return;
  }

  void 
  gnuplot_draw::draw_polycone (std::ostream & out_,
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       const polycone & p_, 
			       size_t arc_sampling_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t phy_sample = arc_sampling_;
    double dphi =  2 * M_PI * CLHEP::radian / phy_sample;

    // outer surface:
    for (size_t i = 0; i <= phy_sample ; ++i) 
      {
	polyline_t polyline_meridian;
	double phi = i * dphi;
	for (polycone::rz_col_t::const_iterator j = p_.points ().begin ();
	     j != p_.points ().end ();
	     j++)
	  {
	    double z = j->first;
	    double r = j->second.rmax;
	    vector_3d P;
	    P.set (r * std::cos (phi), 
		   r * std::sin (phi),  
		   z);
	    vector_3d P2 (P);
	    P2.transform (inverse_rotation);
	    P2 += position_;
	    polyline_meridian.push_back (P2);
	  }
	basic_draw_polyline (out_, polyline_meridian);
      }

    for (polycone::rz_col_t::const_iterator j = p_.points ().begin ();
	 j != p_.points ().end ();
	 j++)
      {
	polyline_t polyline_parallel;
	double z = j->first;
	double r = j->second.rmax;
	for (size_t i = 0; i <= phy_sample ; ++i) 
	  {
	    vector_3d P;
	    double phi = i * dphi;
	    P.set (r * std::cos (phi), 
		   r * std::sin (phi),  
		   z);
	    vector_3d P2 (P);
	    P2.transform (inverse_rotation);
	    P2 += position_;
	    polyline_parallel.push_back (P2);
	  }
	basic_draw_polyline (out_, polyline_parallel);
      }
 
    if (p_.is_extruded ())
      {
	// inner surface:
	for (size_t i = 0; i <= phy_sample ; ++i) 
	  {
	    polyline_t polyline_meridian;
	    double phi = i * dphi;
	    for (polycone::rz_col_t::const_iterator j = p_.points ().begin ();
		 j != p_.points ().end ();
		 j++)
	      {
		double z = j->first;
		double r = j->second.rmin;
		vector_3d P;
		P.set (r * std::cos (phi), 
		       r * std::sin (phi),  
		       z);
		vector_3d P2 (P);
		P2.transform (inverse_rotation);
		P2 += position_;
		polyline_meridian.push_back (P2);
	      }
	    basic_draw_polyline (out_, polyline_meridian);
	  }

	for (polycone::rz_col_t::const_iterator j = p_.points ().begin ();
	     j != p_.points ().end ();
	     j++)
	  {
	    polyline_t polyline_parallel;
	    double z = j->first;
	    double r = j->second.rmin;
	    for (size_t i = 0; i <= phy_sample ; ++i) 
	      {
		vector_3d P;
		double phi = i * dphi;
		P.set (r * std::cos (phi), 
		       r * std::sin (phi),  
		       z);
		vector_3d P2 (P);
		P2.transform (inverse_rotation);
		P2 += position_;
		polyline_parallel.push_back (P2);
	      }
	    basic_draw_polyline (out_, polyline_parallel);
	  }
      }
    return;
  }

  void 
  gnuplot_draw::draw_polyhedra (std::ostream & out_,
				const vector_3d & position_, 
				const rotation_3d & rotation_,
				const polyhedra & p_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    size_t nsides = p_.get_n_sides ();
    double dphi =  2 * M_PI * CLHEP::radian / nsides;
    double phi0 = 0; //M_PI / nsides;
    double factor = 1. / cos (0.5 * dphi);
    for (size_t i = 0; i <= nsides ; ++i) 
      {
	polyline_t polyline_meridian;
	double phi = phi0 + i * dphi;
	for (polyhedra::rz_col_t::const_iterator j = p_.points ().begin ();
	     j != p_.points ().end ();
	     j++)
	  {
	    double z = j->first;
	    double r = factor * j->second.rmax;
	    vector_3d P;
	    P.set (r * std::cos (phi), 
		   r * std::sin (phi),  
		   z);
	    vector_3d P2 (P);
	    P2.transform (inverse_rotation);
	    P2 += position_;
	    polyline_meridian.push_back (P2);
	  }
	basic_draw_polyline (out_, polyline_meridian);
      }
    for (polyhedra::rz_col_t::const_iterator j = p_.points ().begin ();
	 j != p_.points ().end ();
	 j++)
      {
	polyline_t polyline_parallel;
	double z = j->first;
	double r = factor * j->second.rmax;
	for (size_t i = 0; i <= nsides ; ++i) 
	  {
	    vector_3d P;
	    double phi = phi0 + i * dphi;
	    P.set (r * std::cos (phi), 
		   r * std::sin (phi),  
		   z);
	    vector_3d P2 (P);
	    P2.transform (inverse_rotation);
	    P2 += position_;
	    polyline_parallel.push_back (P2);
	  }
	basic_draw_polyline (out_, polyline_parallel);
      }
    if (p_.is_extruded ())
      {
	/*
	cerr << "DEVEL: gnuplot_draw::draw_polyhedra: "
	     << " EXTRUDED !!!" << endl;
	*/
	for (size_t i = 0; i <= nsides ; ++i) 
	  {
	    polyline_t polyline_meridian;
	    double phi = phi0 + i * dphi;
	    bool last_has_zero_r = false;
	    for (polyhedra::rz_col_t::const_iterator j = p_.points ().begin ();
		 j != p_.points ().end ();
		 j++)
	      {
		double z = j->first;
		bool new_has_zero_r = false;
		double r = factor * j->second.rmin;
		if (r == 0)
		  {
		    new_has_zero_r = true;
		  }
		if (new_has_zero_r && last_has_zero_r)
		  {
		    if (polyline_meridian.size () > 2)
		      {
			basic_draw_polyline (out_, polyline_meridian);
		      }
		    polyline_meridian.clear ();
		  }
		vector_3d P;
		P.set (r * std::cos (phi), 
		       r * std::sin (phi),  
		       z);
		vector_3d P2 (P);
		P2.transform (inverse_rotation);
		P2 += position_;
		polyline_meridian.push_back (P2);
		last_has_zero_r = new_has_zero_r;
	      }
	    if (polyline_meridian.size () > 2)
	      {
		basic_draw_polyline (out_, polyline_meridian);
	      }
	  }
	for (polyhedra::rz_col_t::const_iterator j = p_.points ().begin ();
	     j != p_.points ().end ();
	     j++)
	  {
	    polyline_t polyline_parallel;
	    double z = j->first;
	    double r = factor * j->second.rmin;
	    if (r == 0.0) continue;
	    for (size_t i = 0; i <= nsides ; ++i) 
	      {
		vector_3d P;
		double phi = phi0 + i * dphi;
		P.set (r * std::cos (phi), 
		       r * std::sin (phi),  
		       z);
		vector_3d P2 (P);
		P2.transform (inverse_rotation);
		P2 += position_;
		polyline_parallel.push_back (P2);
	      }
	    basic_draw_polyline (out_, polyline_parallel);
	  }
      }
    return;
  }

  void 
  gnuplot_draw::draw_tessellated (std::ostream & out_,
				  const vector_3d & position_, 
				  const rotation_3d & rotation_,
				  const tessellated_solid & t_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();

    polyline_t polyline_facet;
    size_t last_nvtx = 0;
    for (tessellated_solid::facets_col_t::const_iterator i 
	   = t_.facets ().begin ();
	 i !=  t_.facets ().end ();
	 i++)
      {
	polyline_facet.clear ();
	const i_facet * fct = i->second;
	size_t nvtx = fct->get_number_of_vertices ();
	size_t nvtx_safe = nvtx;
	if (last_nvtx == 0)
	  {
	    last_nvtx = nvtx;
	  }
	else if (last_nvtx < 1000)
	  {
	    if (last_nvtx == nvtx)
	      {
		nvtx_safe++;
		last_nvtx = 1000;
	      }
	    else
	      {
		last_nvtx = nvtx;
	      }
	  }
	for (int i = 0; i <= nvtx_safe; i++)
	  {
	    vector_3d P2 (fct->get_vertex (i % nvtx).position);
	    P2.transform (inverse_rotation);
	    P2 += position_;
	    polyline_facet.push_back (P2);
	  }
	basic_draw_polyline (out_, polyline_facet);
      }
    return;
  }

  void 
  gnuplot_draw::draw_intersection_3d (std::ostream & out_,
				      const vector_3d & position_, 
				      const rotation_3d & rotation_,
				      const intersection_3d & i_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();
    const i_composite_shape_3d::shape_t & s1 = i_.get_shape1 ();
    const i_composite_shape_3d::shape_t & s2 = i_.get_shape2 ();    
    const i_shape_3d & sh1 = s1.get_shape ();
    const i_shape_3d & sh2 = s2.get_shape ();
    basic_draw (out_, position_, rotation_, sh1);

    // XXX:

    // draw second shape:
    placement mother_world_placement;
    mother_world_placement.set_translation (position_);
    mother_world_placement.set_orientation (rotation_);
    
    placement world_item_placement;
    mother_world_placement.child_to_mother (s2.get_placement (), 
					    world_item_placement);  
    const vector_3d   & sh2_pos = world_item_placement.get_translation ();
    const rotation_3d & sh2_rot = world_item_placement.get_rotation ();
    basic_draw (out_, 
		sh2_pos, 
		sh2_rot, 
		sh2);
 
    //throw runtime_error ("gnuplot_draw::draw_intersection_3d: Not supported yet !");
    return;
  }

  void 
  gnuplot_draw::draw_union_3d (std::ostream & out_, 
			       const vector_3d & position_, 
			       const rotation_3d & rotation_,
			       const union_3d & u_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert ();
    const i_composite_shape_3d::shape_t & s1 = u_.get_shape1 ();
    const i_composite_shape_3d::shape_t & s2 = u_.get_shape2 ();    
    const i_shape_3d & sh1 = s1.get_shape ();
    const i_shape_3d & sh2 = s2.get_shape ();
    basic_draw (out_, position_, rotation_, sh1);

    // draw second shape:
    placement mother_world_placement;
    mother_world_placement.set_translation (position_);
    mother_world_placement.set_orientation (rotation_);
    
    placement world_item_placement;
    mother_world_placement.child_to_mother (s2.get_placement (), 
					    world_item_placement);  
    const vector_3d   & sh2_pos = world_item_placement.get_translation ();
    const rotation_3d & sh2_rot = world_item_placement.get_rotation ();
    basic_draw (out_, 
		sh2_pos, 
		sh2_rot, 
		sh2);
    //throw runtime_error ("gnuplot_draw::draw_union_3d: Not supported yet !");
    return;
  }

  void 
  gnuplot_draw::draw_subtraction_3d (std::ostream & out_, 
				     const vector_3d & position_, 
				     const rotation_3d & rotation_,
				     const subtraction_3d & s_)
  {
    rotation_3d inverse_rotation (rotation_);
    inverse_rotation.invert (); 
    const i_composite_shape_3d::shape_t & s1 = s_.get_shape1 ();
    const i_composite_shape_3d::shape_t & s2 = s_.get_shape2 ();    
    const i_shape_3d & sh1 = s1.get_shape ();
    const i_shape_3d & sh2 = s2.get_shape ();
    //basic_draw (out_, position_, rotation_, sh1);

    bool draw_first_shape = true;
    if (draw_first_shape)
      { 
	// draw first shape:
	placement mother_world_placement;
	mother_world_placement.set_translation (position_);
	mother_world_placement.set_orientation (rotation_);
	
	placement world_item_placement;
	mother_world_placement.child_to_mother (s1.get_placement (), 
						world_item_placement);  
	const vector_3d   & sh1_pos = world_item_placement.get_translation ();
	const rotation_3d & sh1_rot = world_item_placement.get_rotation ();
	basic_draw (out_, 
		    sh1_pos, 
		    sh1_rot, 
		    sh1);
      }

    bool draw_second_shape = false;
    if (draw_second_shape)
      {
	// draw second shape:
	placement mother_world_placement;
	mother_world_placement.set_translation (position_);
	mother_world_placement.set_orientation (rotation_);
	
	placement world_item_placement;
	mother_world_placement.child_to_mother (s2.get_placement (), 
						world_item_placement);  
	const vector_3d   & sh2_pos = world_item_placement.get_translation ();
	const rotation_3d & sh2_rot = world_item_placement.get_rotation ();
	basic_draw (out_, 
		    sh2_pos, 
		    sh2_rot, 
		    sh2);
      }
    return;
  }

  void gnuplot_draw::basic_draw (ostream & out_,
				 const vector_3d & position_, 
				 const rotation_3d & rotation_, 
				 const i_object_3d & o_,
				 unsigned long mode_)
  { 
    bool mode_wired_cylinder = mode_ & gnuplot_draw::MODE_WIRED_CYLINDER;
    const vector_3d & pos = position_;
    const rotation_3d & rot = rotation_;
    string shape_name = o_.get_shape_name ();

    //cerr << "DEVEL: gnuplot_draw::basic_draw: Entering..." << endl;

    if (o_.has_user_draw ())
      { 
	//cerr << "DEVEL: gnuplot_draw::basic_draw: has_user_draw..." << endl;
	void * user_draw = o_.get_user_draw ();
	gnuplot_draw::draw_user_function_t user_draw_f 
	  = reinterpret_cast<gnuplot_draw::draw_user_function_t> (user_draw);
	//  = static_cast<gnuplot_draw::draw_user_function_t> (user_draw);
	(*user_draw_f) (out_, position_, rotation_, o_, 0);
	return;
      }

    //cerr << "DEVEL: gnuplot_draw::basic_draw: Step 1" << endl;

    if (shape_name == "line_3d")
      {
	const line_3d & l = dynamic_cast<const line_3d &> (o_);
	draw_line (out_, pos, rot, l);
	return;
      }

    if (shape_name == "polyline_3d")
      {
	const polyline_3d & p = dynamic_cast<const polyline_3d &> (o_);
	draw_polyline (out_, pos, rot, p);
	return;
      }

    if (shape_name == "rectangle")
      {
	const rectangle & r = dynamic_cast<const rectangle &> (o_);
	draw_rectangle (out_, pos, rot, r);
	return;
      }

    if (shape_name == "circle")
      {
	const circle & c = dynamic_cast<const circle &> (o_);
	draw_circle (out_, pos, rot, c);
	return;
      }

    if (shape_name == "box")
      {
	const box & b = dynamic_cast<const box &> (o_);
	draw_box (out_, pos, rot, b);
	return;
      }

    if (shape_name == "cylinder")
      {
	const cylinder & c = dynamic_cast<const cylinder &> (o_);
	if (! mode_wired_cylinder)
	  {
	    draw_cylinder (out_, pos, rot, c);
	  }
	else
	  {
	    vector_3d first (0, 0, -c.get_half_z()), last (0, 0, +c.get_half_z());
	    line_3d l (first, last);
	    draw_segment (out_, pos, rot, l);
	  }
	return;
      }

    if (shape_name == "tube")
      {
	const tube & t = dynamic_cast<const tube &> (o_);
	if (! mode_wired_cylinder)
	  {
	    draw_tube (out_, pos, rot, t);
	  }
	else
	  {
	    vector_3d first (0, 0, -t.get_half_z()), last (0, 0, +t.get_half_z());
	    line_3d l (first, last);
	    draw_segment (out_, pos, rot, l);
	  }
	return;
      }

    if (shape_name == "sphere")
      {
	const sphere & s = dynamic_cast<const sphere &> (o_);
	draw_sphere (out_, pos, rot, s);
	return;
      }

    if (shape_name == "polycone")
      {
	//cerr << "DEVEL: gnuplot_draw::basic_draw: shape is polycone..." << endl;
	const polycone & p = dynamic_cast<const polycone &> (o_);
	draw_polycone (out_, pos, rot, p);
	return;
      }

    if (shape_name == "polyhedra")
      {
	const polyhedra & p = dynamic_cast<const polyhedra &> (o_);
	draw_polyhedra (out_, pos, rot, p);
	return;
      }

    if (shape_name == "tessellated")
      {
	const tessellated_solid & t = dynamic_cast<const tessellated_solid &> (o_);
	draw_tessellated (out_, pos, rot, t);
	return;
      }

    if (shape_name == "subtraction_3d")
      {
	const subtraction_3d & s = dynamic_cast<const subtraction_3d &> (o_);
	draw_subtraction_3d (out_, pos, rot, s);
	return;
      }

    clog << "gnuplot_draw::basic_draw: "
	 << " No method to draw an 3D object of type '" 
	 << shape_name << "' !" << endl;
    return;
  }

  void gnuplot_draw::draw (ostream & out_, 
			   const i_placement & p_, 
			   const i_object_3d & o_,
			   unsigned long mode_)
  { 
    bool mode_wired_cylinder = mode_ & gnuplot_draw::MODE_WIRED_CYLINDER;

    // multi-placement:
    for (int i = 0; i < p_.get_number_of_items (); i++)
      {
	placement p;
	p_.get_placement (i, p);
	const vector_3d & pos = p.get_translation ();
	const rotation_3d & rot = p.get_rotation ();
	basic_draw (out_, pos, rot, o_, mode_);
	return;
      }
  }

  void gnuplot_draw::draw (ostream & out_, 
			   const i_placement & p_, 
			   const i_object_3d & o_)
  {
    gnuplot_draw::draw (out_, p_, o_, gnuplot_draw::MODE_NULL);
  }

} // end of namespace geomtools

// end of gnuplot_draw.cc
