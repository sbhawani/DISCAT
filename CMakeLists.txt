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

# Apply ROOT flags (headers + defs)
separate_arguments(ROOT_CFLAGS)
add_compile_options(${ROOT_CFLAGS})

# --- Find all MACP shared libs (_h.so files) ---
file(GLOB_RECURSE MACP_SHARED_LIBS "${MACP_PATH}/core/*_h.so")
message(STATUS "Found the following _h.so libraries: ${MACP_SHARED_LIBS}")

# --- Copy to lib*.so (NOT replacing _h.so) ---
set(COPIED_LIBRARIES "")
foreach(lib ${MACP_SHARED_LIBS})
    get_filename_component(lib_name ${lib} NAME)
    string(REPLACE "_h.so" ".so" new_name ${lib_name})
    set(new_path "${CMAKE_BINARY_DIR}/copied_libs/lib${new_name}")

    add_custom_command(
        OUTPUT ${new_path}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/copied_libs"
        COMMAND ${CMAKE_COMMAND} -E copy ${lib} ${new_path}
        COMMENT "Copying ${lib} -> ${new_path}"
        VERBATIM
    )

    list(APPEND COPIED_LIBRARIES ${new_path})
endforeach()

# --- Create custom target to ensure copying happens ---
add_custom_target(CopyMacpLibs ALL DEPENDS ${COPIED_LIBRARIES})

# --- Find ROOT without Python support ---
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)

# --- Source files ---
add_executable(discat
    examples/main.cpp
    examples/RunDVCSmodel.cpp
    core/DISCAT.cxx
    core/BinManager.cxx
    core/DISCATMath.cxx
    core/FastMCModel.cxx
    core/DVCSplotter.cxx
)

add_dependencies(discat CopyMacpLibs)

# --- Link CLAS12 and MACP directories ---
link_directories(${CLAS12ROOT_PATH}/lib)

# --- Link libraries ---
target_link_libraries(discat
    PRIVATE
    ${ROOT_LIBS}
    ${COPIED_LIBRARIES}
    ROOT::ROOTTPython
)

# --- Set RPATH so it can find copied libraries at runtime ---
set_target_properties(discat PROPERTIES
    BUILD_RPATH "${CMAKE_BINARY_DIR}/copied_libs"
    INSTALL_RPATH "${CMAKE_BINARY_DIR}/copied_libs"
)
