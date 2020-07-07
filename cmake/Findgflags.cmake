# - Try to find GFLAGS
#
# The following variables are optionally searched for defaults
#  GFLAGS_ROOT_DIR:            Base directory where all GFLAGS components are found
#
# The following are set after configuration is done:
#  GFLAGS_FOUND
#  GFLAGS_INCLUDE_DIRS
#  GFLAGS_LIBRARIES
#  GFLAGS_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

# Support preference of static libs by adjusting CMAKE_FIND_LIBRARY_SUFFIXES
if( GFlags_USE_STATIC_LIBS )
  set( _gflags_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
  if(WIN32)
    list(INSERT CMAKE_FIND_LIBRARY_SUFFIXES 0 .lib .a)
  else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
  endif()
endif()

set(GFLAGS_ROOT_DIR "" CACHE PATH "Folder contains Gflags")

# We are testing only a couple of files in the include directories
if(WIN32)
    find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h
        PATHS ${GFLAGS_ROOT_DIR}/src/windows)
else()
    find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h
        PATHS ${GFLAGS_ROOT_DIR})
endif()

if(MSVC)
    find_library(GFLAGS_LIBRARY_RELEASE
        NAMES libgflags
        PATHS ${GFLAGS_ROOT_DIR}
        PATH_SUFFIXES Release)

    find_library(GFLAGS_LIBRARY_DEBUG
        NAMES libgflags-debug
        PATHS ${GFLAGS_ROOT_DIR}
        PATH_SUFFIXES Debug)

    set(GFLAGS_LIBRARY optimized ${GFLAGS_LIBRARY_RELEASE} debug ${GFLAGS_LIBRARY_DEBUG})
else()
    find_library(GFLAGS_LIBRARY gflags)
endif()

find_package_handle_standard_args(GFlags DEFAULT_MSG GFLAGS_INCLUDE_DIR GFLAGS_LIBRARY)

if( GFlags_USE_STATIC_LIBS )
  set( CMAKE_FIND_LIBRARY_SUFFIXES ${_gflags_ORIG_CMAKE_LIBRARY_SUFFIXES})
endif()

if(GFLAGS_FOUND)
    set(GFLAGS_INCLUDE_DIRS ${GFLAGS_INCLUDE_DIR})
    set(GFLAGS_LIBRARIES ${GFLAGS_LIBRARY})
    message(STATUS "Found gflags  (include: ${GFLAGS_INCLUDE_DIR}, library: ${GFLAGS_LIBRARY})")
    mark_as_advanced(GFLAGS_LIBRARY_DEBUG GFLAGS_LIBRARY_RELEASE
                     GFLAGS_LIBRARY GFLAGS_INCLUDE_DIR GFLAGS_ROOT_DIR)
endif()
