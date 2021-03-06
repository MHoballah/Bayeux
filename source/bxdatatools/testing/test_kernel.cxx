// test_kernel.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <datatools/datatools.h>
#include <datatools/kernel.h>
#include <datatools/exception.h>
#include <datatools/library_info.h>

int main(int argc_, char * argv_[]) {
  datatools::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {

    DT_THROW_IF(! datatools::kernel::is_instantiated(),
                std::runtime_error,
                "No datatools kernel is instantiated !");

    const datatools::kernel & dtkl = datatools::kernel::const_instance();
    dtkl.tree_dump(std::cout, "The datatools' kernel: ", "*** ");

    if (dtkl.has_library_info_register()) {
      const datatools::library_info & lib_info_reg
        = dtkl.get_library_info_register();
      lib_info_reg.tree_dump(std::cout, "Kernel's library info register: ");
    }

    std::cerr << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  datatools::terminate();
  return error_code;
}
