// -*- mode: c++; -*- 
// test_ioutils.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/ioutils.h>

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      std::clog << "Test of the 'ioutils' class..." << std::endl; 
  
      bool debug = false;
      bool color = false;

      int iarg = 1;
      while (iarg <  argc_)  
        {
          std::string arg = argv_[iarg]; 

          if (arg == "-d" || arg == "--debug") debug =  true;

          if (arg == "-c" || arg == "--color") color =  true;

          iarg++;
        }
    
      datatools::io::set_colored (color);
      std::clog << datatools::io::indent << datatools::io::warning << "This is a warning print ! " 
                << datatools::io::normal << std::endl;
      datatools::io::indent++;
      std::clog << datatools::io::indent << datatools::io::notice << "This is a notice print ! " 
                << datatools::io::normal << std::endl;
      std::clog << datatools::io::indent << datatools::io::debug << "This is a debug print ! " 
                << datatools::io::normal << std::endl;
      datatools::io::indent++;
      std::clog << datatools::io::indent << datatools::io::normal << datatools::io::notice << "More print ! " 
                << datatools::io::normal << std::endl;
      std::clog << datatools::io::indent << datatools::io::normal << datatools::io::notice << "Yet another print ! " 
                << datatools::io::normal << std::endl;
      datatools::io::indent--;
      std::clog << datatools::io::indent << datatools::io::normal << datatools::io::notice << "Back to margin ! " 
                << datatools::io::normal << std::endl;
      datatools::io::indent (0);
      std::clog << datatools::io::indent << datatools::io::devel << "Done." 
                << datatools::io::normal << std::endl;
      datatools::io::indent (6);  
      std::clog << datatools::io::indent << datatools::io::error << "Ooops !" 
                << datatools::io::normal << std::endl;

      datatools::io::indenter ind;
      ind++;
      std::cout << ind << "Hello, World !" << std::endl;
      ind++;
      std::cout << ind << "How do you feel ?" << std::endl;
      ind (4);
      std::cout << ind << "Well, I'm fine." << std::endl;
      ind--;
      std::cout << ind << "Good ! Let's go !" << std::endl;

      uint32_t value = 54;
      std::cout << "value (dec) = " << value << std::endl;
      std::cout << "value (bin) = " << datatools::io::to_binary (value) << std::endl << std::endl;
      value = 55;
      std::cout << "value (dec) = " << value << std::endl;
      std::cout << "value (bin) = " << datatools::io::to_binary (value) << std::endl;
    }
  catch ( std::exception & x)
    {
      std::cerr << "error: " << x.what() << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_ioutils.cxx
