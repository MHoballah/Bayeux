#!/usr/bin/env bash

# A Bash script to build and install devel Bayeux on Ubuntu (16.04/18.04/20.04) or CentOS (7.5).

opwd=$(pwd)

function my_exit()
{
    local error_code=$1
    if [ "x${error_code}" = "x" ]; then
	error_code=0
    fi
    shift 1
    local error_msg="$@"
    if [ ${error_code} -ne 0 -a -n "${error_msg}" ]; then
	echo >&2 "[error] ${error_msg}"
    fi
    exit ${error_code}
}

function my_usage()
{
    cat<<EOF

Options:

   --help                  : Print help
   --debug                 : Activate debug mode
   --dry-run               : Dry run
   --only-configure        : Perform configuration stage only
   --nprocs N              : Use N processors
   --install-base-dir path : Select the Bayeux installation directory
   --build-base-dir path   : Select the Bayeux build directory
   --clean-build-dir       : Clean the the Bayeux build directory 
                             after successfull installation
   --with-bxdecay0         : Use the BxDecay0 library
   --without-geant4        : Do not build the Geant4 module
   --with-geant4           : Build the Geant4 module
   --with-geant4-experimental : 
                             Build the Geant4 module (experimental mode).
			     Allows Geant4 10.5
   --without-qt            : Do not build the Qt-based GUI material
   --boost-root path       : Set the CAMP prefix path
   --camp-prefix path      : Set the CAMP prefix path
   --camp-legacy           : Allow legacy version of CAMP (0.8.0)
   --minimal-build         : Minimal build
   --bayeux-suffix name    : Set a special suffix for the build directory
   --gcc-version version   : Set GC version

EOF
    return
}

nprocs=4
dry_run=False
rebuild=0
only_configure=0
debug=0
bayeux_source_dir="${opwd}"
bayeux_version="develop"
install_base_dir=$(pwd)/_install.d
build_base_dir=$(pwd)/_build.d
clean_build_dir=false
bayeux_suffix=
with_bxdecay0=false
with_geant4=true
with_geant4_experimental=false
with_qt=true
camp_prefix=""
camp_legacy=false
boost_root=""
minimal_build=false
gcc_version=
use_ninja=false

function cl_parse()
{
    while [ -n "$1" ]; do
	local arg="$1"
	if [ "${arg}" = "--help" ]; then
	    my_usage
	    my_exit 0
	elif [ "${arg}" = "--debug" ]; then
	    debug=1
	elif [ "${arg}" = "--dry-run" ]; then
	    dry_run=True
	elif [ "${arg}" = "--minimal-build" ]; then
	    minimal_build=true
	elif [ "${arg}" = "--rebuild" ]; then
	    rebuild=1
	elif [ "${arg}" = "--only-configure" ]; then
	    only_configure=1
	elif [ "${arg}" = "--nprocs" ]; then
	    shift 1
	    nprocs=$1
	elif [ "${arg}" = "--install-base-dir" ]; then
	    shift 1
	    install_base_dir="$1"
	elif [ "${arg}" = "--build-base-dir" ]; then
	    shift 1
	    build_base_dir="$1"
	elif [ "${arg}" = "--bayeux-suffix" ]; then
	    shift 1
	    bayeux_suffix="$1"
	elif [ "${arg}" = "--boost-root" ]; then
	    shift 1
	    boost_root="$1"
	elif [ "${arg}" = "--clean-build-dir" ]; then
	    clean_build_dir=true
	elif [ "${arg}" = "--with-bxdecay0" ]; then
	    with_bxdecay0=true
	elif [ "${arg}" = "--with-geant4" ]; then
	    with_geant4=true
	elif [ "${arg}" = "--with-geant4-experimental" ]; then
	    with_geant4=true
	    with_geant4_experimental=true
	elif [ "${arg}" = "--without-geant4" ]; then
	    with_geant4=false
	    with_geant4_experimental=false
	elif [ "${arg}" = "--without-qt" ]; then
	    with_qt=false
	elif [ "${arg}" = "--camp-prefix" ]; then
	    shift 1
	    camp_prefix="$1"
	elif [ "${arg}" = "--camp-legacy" ]; then
	    camp_legacy=true
	elif [ "${arg}" = "--gcc-version" ]; then
	    shift 1
	    gcc_version="$1"
	fi
	shift 1
    done
    return 0
}

if [ "x${gcc_version}" != "x" ]; then
    export CC=$(which gcc-${gcc_version})
    export CXX=$(which g++-${gcc_version})
fi

cl_parse $@
if [ $? -ne 0 ]; then
    echo >&2 "[error] Command line parsing failed!"
    my_exit 1
fi

echo >&2 "[info] Bayeux source dir : '${bayeux_source_dir}'"
echo >&2 "[info] minimal_build     : ${minimal_build}"
echo >&2 "[info] GCC version       : '${gcc_version}'"
echo >&2 "[info] with_geant4       : ${with_geant4}"
echo >&2 "[info] \`-- experimental  : ${with_geant4_experimental}"
echo >&2 "[info] camp_prefix       : '${camp_prefix}'"
echo >&2 "[info] camp_legacy       : '${camp_legacy}'"
echo >&2 "[info] with_qt           : ${with_qt}"
echo >&2 "[info] with_bxdecay0     : ${with_bxdecay0}"
echo >&2 "[info] boost_root        : ${boost_root}"
echo >&2 "[info] bayeux_suffix     : ${bayeux_suffix}"
# my_exit 1

# Check distribution:
if [ -f /etc/lsb-release ]; then
    distrib_id=$(cat /etc/lsb-release | grep DISTRIB_ID | cut -d= -f2)
    distrib_release=$(cat /etc/lsb-release | grep DISTRIB_RELEASE | cut -d= -f2)
    
    if [ "${distrib_id}" != "Ubuntu" ]; then
	echo >&2 "[error] Not an Ubuntu Linux!"
	my_exit 1
    fi
    
    if [ "${distrib_release}" != "16.04" -a "${distrib_release}" != "18.04" -a "${distrib_release}" != "20.04" ]; then
	echo >&2 "[error] Not an Ubuntu Linux version 16.04, 18.04 or 20.04! Abort!"
	my_exit 1
    else
	echo >&2 "[info] Found Ubuntu Linux ${distrib_release}"
    fi
elif [ -f /etc/redhat-release ]; then
    distrib_id=$(cat /etc/redhat-release | head -1 | cut -d' ' -f1) 
    distrib_release=$(cat /etc/redhat-release | head -1 | cut -d' ' -f4 | cut -d'.' -f1,2) 
   
    if [ "${distrib_id}" != "CentOS" ]; then
	echo >&2 "[error] Not an CentOS Linux!"
	my_exit 1
    fi
    
    if [ "${distrib_release}" != "7.5" -a  "${distrib_release}" != "7.7" ]; then
	echo >&2 "[error] Not an CentOS Linux version 7.5 or 7.7! Abort!"
	my_exit 1
    else
	echo >&2 "[info] Found CentOS Linux ${distrib_release}"
    fi
fi

# Check Git source repository:
echo >&2 "[info] Bayeux source directory: '${bayeux_source_dir}'"
ls -l ${bayeux_source_dir}/.git
if [ ! -d ${bayeux_source_dir}/.git ]; then
    echo >&2 "[error] Not the Bayeux source directory."
    app_exit 1
fi
bx_branch=$(git --git-dir=${bayeux_source_dir}/.git --work-tree=${bayeux_source_dir} branch | grep \* | tr -d [[:space:]\*] )
echo >&2 "[info] Bayeux Git branch: '${bx_branch}'"
bayeux_version="${bx_branch}"

install_dir=${install_base_dir}/${bayeux_version}${bayeux_suffix}
build_dir=${build_base_dir}/${bayeux_version}${bayeux_suffix}

which cmake > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[error] CMake not found!"
    my_exit 1
else
    echo >&2 "[info] CMake $(cmake --version | head -1  | cut -d' ' -f3) found at $(which cmake)!"
fi

make_bin=
which ninja > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo >&2 "[warning] ninja builder not found!"
    # my_exit 1
else
    echo >&2 "[info] Found ninja  : $(which ninja)"
    use_ninja=true
    make_bin=$(which ninja)
fi
if [ "x${make_bin}" = "x" ]; then
    make_bin=$(which make)
fi

clhep_checked=false
which clhep-config > /dev/null 2>&1
if [ $? -eq 0 ]; then
    clhep_checked=true
else
    echo >&2 "[info] Found CLHEP  : $(which clhep-config)"
fi
if [ ${clhep_checked} = false ]; then
    echo >&2 "[error] CLHEP is not setup!"
    my_exit 1
fi

if [ ${minimal_build} == false -a ${with_geant4} == true ]; then
    which geant4-config > /dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo >&2 "[error] GEANT4 is not setup!"
	my_exit 1
    else
	echo >&2 "[info] Found GEANT4 : $(which geant4-config)"
    fi
fi

if [ ${minimal_build} == false ]; then
    which root-config > /dev/null 2>&1
    if [ $? -ne 0 ]; then
	echo >&2 "[error] ROOT is not setup!"
	my_exit 1
    else
	echo >&2 "[info] Found ROOT   : $(which root-config)"
    fi
fi

if [ ! -d ${bayeux_source_dir} ]; then
    echo >&2 "[error] Bayeux source directory '${bayeux_source_dir}' does not exist! Abort!"
    my_exit 1
fi

if [ -d ${install_dir} ]; then
    rm -fr ${install_dir}
fi

if [ ! -d ${build_dir} ]; then
    mkdir -p ${build_dir}
else
    if [ ${rebuild} -eq 1 ]; then
	echo >&2 "[info] Rebuilding Bayeux..."
	rm -fr ${build_dir}
	mkdir -p ${build_dir}
    fi
fi

if [ "x${boost_root}" = "x" ]; then
    which spack > /dev/null 2>&1
    if [ $? -eq 0 ]; then
	boost_root=$(spack find --format '{prefix}' boost)
    fi
fi

if [ "x${boost_root}" = "x" ]; then
    if [ "x${BX_BOOST_INSTALL_DIR}" != "x" ]; then
	boost_root="${BX_BOOST_INSTALL_DIR}"
    # else
    # 	boost_root="/usr"
    fi
fi
if [ ! -d ${boost_root} ]; then
      my_exit 1 "Boost root '${boost_root}' does not exist!"
fi
boost_option="-DBOOST_ROOT=${boost_root} -DBoost_ADDITIONAL_VERSIONS=1.69"

if [ "x${camp_prefix}" = "x" ]; then
    dpkg -l | grep libcamp-dev
    if [ $? -eq 0 ]; then
	camp_prefix="/usr"
    fi
fi
if [ "x${camp_prefix}" = "x" ]; then
    which spack  > /dev/null 2>&1
    if [ $? -eq 0 ]; then
	camp_prefix=$(spack find --format '{prefix}' camp)
    fi
fi
if [ "x${camp_prefix}" = "x" ]; then
    if [ "x${BX_CAMP_INSTALL_DIR}" != "x" ]; then
	camp_prefix="${BX_CAMP_INSTALL_DIR}"
    fi
fi
if [ "x${camp_prefix}" = "x" ]; then
    my_exit 1 "Missing CAMP prefix!"   
fi
camp_dir="${camp_prefix}/lib/camp/cmake"
if [ ! -d ${camp_dir} ]; then
    my_exit 1 "CAMP dir '${camp_dir}' does not exist!"
fi
camp_option="-DCAMP_DIR=${camp_dir}"
if [ ${camp_legacy} = true ]; then
    camp_option="${camp_option} -DBAYEUX_CAMP_LEGACY=ON"
fi
clhep_prefix=""
which clhep-config > /dev/null 2>&1 
if [ $? -eq 0 ]; then
    clhep_prefix="$(clhep-config --prefix | tr -d '"')"
else
    which spack > /dev/null 2>&1
    if [ $? -eq 0 ]; then
	clhep_prefix=$(spack find --format "{prefix}" "clhep")
    fi
fi
if [ "x${clhep_prefix}" = "x" ]; then
    my_exit 1 "Missing CLHEP prefix!"   
fi
export PATH="${clhep_prefix}/bin:${PATH}"
if [ "${clhep_prefix}" = "/usr" ]; then
    clhep_dir="/usr/lib/x86_64-linux-gnu"
    clhep_option="-DCLHEP_ROOT_DIR=${clhep_prefix}"
else
    clhep_dir="${clhep_prefix}/lib/CLHEP-$(clhep-config --version | cut -d' ' -f2)"
fi
if [ ! -d ${clhep_dir} ]; then
      my_exit 1 "CLHEP dir '${clhep_dir}' does not exist!"
fi
if [ "x${clhep_option}" = "x" ]; then
    clhep_option="-DCLHEP_DIR=${clhep_dir}"
fi

bxdecay0_options="-DBAYEUX_WITH_BXDECAY0=OFF"

# bxdecay0_dir="/opt/sw/Spack/install.d/linux-ubuntu18.04-ivybridge/gcc-7.4.0/bxdecay0-1.0.0-beta2-swxftf7we6v5rzez265ujc4spd7oobf2"
# bxdecay0_dir="/home/mauger/Documents/Private/Software/BxCppDev/BxDecay0/bxdecay0/_install.d/lib/cmake/BxDecay0-1.0.0/"
bxdecay0_dir=""
if [ ${with_bxdecay0} = true ]; then
    
    if [ "x${bxdecay0_dir}" = "x" ]; then 
	which spack > /dev/null 2>&1 
	if [ $? -eq 0 ]; then
	    bxdecay0_dir="$(spack find --format '{prefix}' bxdecay0)/lib/cmake"
	fi
    fi
    
    if [ "x${bxdecay0_dir}" = "x" ]; then 
	which bxdecay0-query> /dev/null 2>&1 
	if [ $? -eq 0 ]; then
	    bxdecay0_dir="$(bxdecay0-query --prefix)/lib/cmake"
	fi
    fi
   
    if [ "x${bxdecay0_dir}" = "x" ]; then 
	pkg-config --exists bxdecay0> /dev/null 2>&1
	if [ $? -eq 0 ]; then
	    bxdecay0_dir="$(pkg-config pkg-config --variable=prefix bxdecay0)/lib/cmake"
	fi
    fi

    if [ "x${bxdecay0_dir}" = "x" ]; then 
      my_exit 1 "BxDecay0 was not found!"
    fi

    if [ ! -d "${bxdecay0_dir}" ]; then 
      my_exit 1 "BxDecay0 dir '${bxdecay0_dir}' does not exists!"
    fi

    bxdecay0_options="-DBAYEUX_WITH_BXDECAY0=ON -DBxDecay0_DIR=${bxdecay0_dir}"
    
fi
qt_option0=
qt_option1=
qt_option2=
qt_option3=
qt_option4=
if [ ${minimal_build} == false -a ${with_qt} == true ]; then
    qt5_dir="/usr/lib/x86_64-linux-gnu/cmake"
    qt5core_dir="${qt5_dir}/Qt5Core"
    qt5gui_dir="${qt5_dir}/Qt5Gui"
    qt5widgets_dir="${qt5_dir}/Qt5Widgets"
    qt5svg_dir="${qt5_dir}/Qt5Svg"
    if [ ! -d ${qt5core_dir} ]; then
	my_exit 1 "Qt5Core dir '${qt5core_dir}' does not exist!"
    fi
    if [ ! -d ${qt5gui_dir} ]; then
	my_exit 1 "Qt5Gui dir '${qt5gui_dir}' does not exist!"
    fi
    if [ ! -d ${qt5widgets_dir} ]; then
	my_exit 1 "Qt5Widgets dir '${qt5widgets_dir}' does not exist!"
    fi
    if [ ! -d ${qt5svg_dir} ]; then
	echo >&2 "[warning] Qt5Svg dir '${qt5svg_dir}' does not exist!"
	# my_exit 1 "Qt5Svg dir '${qt5svg_dir}' does not exist!"
    fi
    qt_option0="-DBAYEUX_WITH_QT_GUI=ON"
    qt_option1="-DQt5Core_DIR=${qt5core_dir}"
    qt_option2="-DQt5Gui_DIR=${qt5gui_dir}"
    qt_option3="-DQt5Widgets_DIR=${qt5widgets_dir}"
    qt_option4="-DQt5Svg_DIR=${qt5svg_dir}"
    if [ ! -d ${qt5svg_dir} ]; then    
	qt_option4=""
    fi
else
    qt_option0="-DBAYEUX_WITH_QT_GUI=OFF"
fi

root_prefix="$(root-config --prefix)"
root_dir="${root_prefix}/share/root/cmake"
root_option="-DROOT_DIR=${root_dir}"

geant4_option=""
if [ ${minimal_build} == false -a ${with_geant4} == true ]; then
    geant4_prefix="$(geant4-config --prefix)"
    # if [ "x${BX_GEANT4_INSTALL_DIR}" != "x" ]; then
    #     geant4_prefix="${BX_GEANT4_INSTALL_DIR}"
    # fi
    geant4_dir="${geant4_prefix}/lib/Geant4-$(geant4-config --version | cut -d' ' -f2)"
    if [ ! -d ${geant4_dir} ]; then
	my_exit 1 "GEANT4 dir '${geant4_dir}' does not exist!"
    fi
    geant4_option="-DBAYEUX_WITH_GEANT4_MODULE=ON -DGeant4_DIR=${geant4_dir}"
    if [ ${with_geant4_experimental} == true ]; then
	geant4_option="${geant4_option} -DBAYEUX_WITH_GEANT4_EXPERIMENTAL=ON"
    fi
else
    geant4_option="-DBAYEUX_WITH_GEANT4_MODULE=OFF"
fi

cmake_prefix_path_option=
# export CMAKE_MODULE_PATH=${root_module_path}:${CMAKE_MODULE_PATH}
# echo >&2 "[info] CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}"
# cmake_prefix_path_option="-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}"

echo >&2 "Bayeux build context: "
echo >&2 " - bayeux_version   = ${bayeux_version}"
echo >&2 " - build_dir        = ${build_dir}"
echo >&2 " - install_dir      = ${install_dir}"
echo >&2 " - Boost root       = ${boost_root}"
echo >&2 " - CAMP dir         = ${camp_dir}"
echo >&2 " - CLHEP dir        = ${clhep_dir}"
echo >&2 " - ROOT dir         = ${root_dir}"
if [ ${with_qt} == true ]; then
    echo >&2 " - Qt5 dir          = ${qt5_dir}"
fi
if [ ${with_geant4} == true ]; then
    echo >&2 " - GEANT4 dir       = ${geant4_dir}"
fi
if [ ${with_bxdecay0} == true ]; then
    echo >&2 " - BxDecay0 dir     = ${bxdecay0_dir}"
fi

if [ ${dry_run} = True ]; then
      my_exit 0
fi

default_with_module=ON
minimal_build_option=
if [ ${minimal_build} == true ]; then
    default_with_module=OFF
    minimal_build_option="-DBAYEUX_MINIMAL_BUILD=ON"
    root_option=
    geant4_option=
    qt_option0=
    qt_option1=
    qt_option2=
    qt_option3=
    qt_option4=
fi

compiler_options=
if [ "x${gcc_version}" != "x" ]; then
    compiler_options="\
    -DCMAKE_C_COMPILER=/usr/bin/gcc-${gcc_version} \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++-${gcc_version} \
    "
fi

# bayeux_mcnp_option="-DBAYEUX_WITH_MCNP_MODULE=OFF"
bayeux_mcnp_option=""

ninja_option=
if [ ${use_ninja} = true ]; then
    ninja_option="-GNinja"
fi

cd ${build_dir}
echo >&2 ""
echo >&2 "[info] Configuring..."
cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:PATH="${install_dir}" \
    ${cmake_prefix_path_option} \
    ${compiler_options} \
    ${boost_option} \
    ${camp_option} \
    ${clhep_option} \
    ${minimal_build_option} \
    ${root_option} \
    -DBAYEUX_COMPILER_ERROR_ON_WARNING=ON \
    -DBAYEUX_WITH_IWYU_CHECK=OFF \
    -DBAYEUX_WITH_DEVELOPER_TOOLS=ON \
    -DBAYEUX_WITH_BRIO=${default_with_module} \
    -DBAYEUX_WITH_CUTS=${default_with_module} \
    -DBAYEUX_WITH_MYGSL=${default_with_module} \
    -DBAYEUX_WITH_MATERIALS=${default_with_module} \
    -DBAYEUX_WITH_DPP=${default_with_module} \
    -DBAYEUX_WITH_GEOMTOOLS=${default_with_module} \
    -DBAYEUX_WITH_EMFIELD=${default_with_module} \
    -DBAYEUX_WITH_GENBB=${default_with_module} \
    ${bxdecay0_options} \
    -DBAYEUX_WITH_GENVTX=${default_with_module} \
    -DBAYEUX_WITH_MCTOOLS=${default_with_module} \
    -DBAYEUX_WITH_LAHAGUE=OFF \
    ${bayeux_mcnp_option} \
    ${geant4_option} \
    ${qt_option0} \
    ${qt_option1} \
    ${qt_option2} \
    ${qt_option3} \
    ${qt_option4} \
    -DBAYEUX_ENABLE_TESTING=ON \
    -DBAYEUX_WITH_DOCS=ON \
    -DBAYEUX_WITH_DOCS_OCD=ON \
    ${ninja_option} \
    ${bayeux_source_dir}
if [ $? -ne 0 ]; then
    echo >&2 "[error] Bayeux ${bayeux_version} configuration failed!"
    my_exit 1
fi
 
if [ ${only_configure} -eq 0 ]; then

    echo >&2 ""
    echo >&2 "[info] Building..."
    ${make_bin} -j${nprocs}
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux ${bayeux_version} build failed!"
	my_exit 1
    fi

    echo >&2 ""
    echo >&2 "[info] Testing..."
    ${make_bin} test
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux ${bayeux_version} testing failed!"
	my_exit 1
    fi

    echo >&2 ""
    echo >&2 "[info] Installing..."
    ${make_bin} install
    if [ $? -ne 0 ]; then
	echo >&2 "[error] Bayeux ${bayeux_version} installation failed!"
	my_exit 1
    else
	if [ ${clean_build_dir} == true ]; then
	    rm -fr ${build_dir}
	fi
    fi
    
fi

my_exit 0

# end
