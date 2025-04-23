cmake_minimum_required(VERSION 3.16)
project(DISCATProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Check for required environment variables ---
if(NOT DEFINED ENV{CLAS12ROOT})
    message(FATAL_ERROR "CLAS12ROOT environment variable not set!")
endif()

if(NOT DEFINED ENV{MACP})
    message(FATAL_ERROR "MACP environment variable not set!")
endif()

# --- Setup Paths ---
set(CLAS12ROOT_PATH $ENV{CLAS12ROOT})
set(MACP_PATH $ENV{MACP})

# --- ROOT flags via root-config ---
execute_process(
    COMMAND root-config --cflags
    OUTPUT_VARIABLE ROOT_CFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND root-config --libs
    OUTPUT_VARIABLE ROOT_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# --- Include directories ---
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CLAS12ROOT_PATH}/Clas12Root)
include_directories(${MACP_PATH}/core)
include_directories("/cvmfs/oasis.opensciencegrid.org/jlab/hallb/clas12/sw/almalinux9-gcc11/local/root/6.30.04/include")
include_directories(${MACP_PATH}/core)

# Apply ROOT flags (headers + defs)
separate_arguments(ROOT_CFLAGS)
add_compile_options(${ROOT_CFLAGS})

# --- Find all MACP shared libs (.so) ---
file(GLOB_RECURSE MACP_SHARED_LIBS "${MACP_PATH}/core/*_h.so")
message(STATUS "MACP libraries found: ${MACP_SHARED_LIBS}")
file(GLOB MACP_LIBS "/w/hallb-scshelf2102/clas12/singh/HighLevelAN/FastMC/Fast_mc_setup/macparticles/core/*.so")

find_package(ROOT REQUIRED COMPONENTS Core RIO Tree ROOTTPython)



# --- Source files ---
add_executable(discat
    examples/main.cpp
    examples/RunDVCSmodel.cpp
    core/DISCAT.cxx
    core/FastMCModel.cxx
)

# --- Link CLAS12 and MACP directories ---
link_directories(${CLAS12ROOT_PATH}/lib)

# --- Link ROOT ---
target_link_libraries(discat 
PRIVATE 
${MACP_LIBS}
${ROOT_LIBS}
ROOT::ROOTTPython
)


# --- Link MACP shared libraries directly (using full paths) ---
# --- Also ensure that MACP shared libraries are available during runtime ---
set_target_properties(discat PROPERTIES
    BUILD_RPATH "${MACP_PATH}/core"
    INSTALL_RPATH "${MACP_PATH}/core"
)


