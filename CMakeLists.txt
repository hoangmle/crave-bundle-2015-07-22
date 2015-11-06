project ( crave-bundle )
cmake_minimum_required(VERSION 2.8)

option(CRAVE_ENABLE_EXPERIMENTAL "enable experimental extensions of CRAVE" off)

if (CRAVE_ENABLE_EXPERIMENTAL)
  add_definitions(-std=c++11)
endif()

option(CRAVE_ENABLE_COVERAGE "enable code coverage for CRAVE tests" off)  

if (CRAVE_ENABLE_COVERAGE)
  add_definitions(--coverage)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage") 
endif()

if (metaSMT_USE_Boolector)
  add_definitions(-DmetaSMT_USE_Boolector)
endif()
if (metaSMT_USE_CVC4)
  add_definitions(-DmetaSMT_USE_CVC4)
endif()
if (metaSMT_USE_Z3)
  add_definitions(-DmetaSMT_USE_Z3)
endif()
if (metaSMT_USE_SWORD)
  add_definitions(-DmetaSMT_USE_SWORD)
endif()
if (metaSMT_USE_CUDD)
  add_definitions(-DmetaSMT_USE_CUDD)
endif()

if (WITH_SYSTEMC)
  add_definitions(-DWITH_SYSTEMC)
endif()

include(CTest)

enable_testing()
list(INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_BINARY_DIR}")
list(INSERT CMAKE_PREFIX_PATH 0 "${metaSMT_BINARY_DIR}/src")

add_subdirectory ( metaSMT )
add_subdirectory ( crave )

find_package(metaSMT QUIET)

if (WITH_SYSTEMC)
  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/crave/cmake)
  find_package(SystemC REQUIRED)
endif()

add_subdirectory(examples)
add_subdirectory(gcovFilter)

option(ENABLE_CRAVE_BUNDLE_TESTS "enable auto-generated tests from examples" off)

if (ENABLE_CRAVE_BUNDLE_TESTS)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
  endif()
endif()

############################################################
##                CPack settings                          ##
############################################################

option( CRAVE_BUNDLE_PACKAGE_BY_DATE "use date for package name instead of git description" on)

if( CRAVE_BUNDLE_PACKAGE_BY_DATE )

  # CPack version numbers for release tarball name.
  if(UNIX)
  execute_process(COMMAND date +%Y OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_MAJOR ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND date +%m OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_MINOR ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND date +%d OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_PATCH ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
  set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}-${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}")
else()
  include(cmake/get_git_version.cmake)
  get_git_version(VERSION crave-bundle-)
endif()

message(STATUS "crave-bundle VERSION: ${VERSION}")


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CRAVE - Constrained RAndom Verification Environment for SystemC/C++")
set(CPACK_PACKAGE_VENDOR "CRAVE development team")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_GENERATOR TGZ)
set(
CPACK_SOURCE_PACKAGE_FILE_NAME
"${PROJECT_NAME}-${VERSION}"
CACHE INTERNAL "tarball basename"
)
set(CPACK_SOURCE_GENERATOR TGZ)

set(CPACK_SOURCE_IGNORE_FILES
"/\\\\.git"
"/deps.*/"
"/build.*/"
"/root.*/"
"^${PROJECT_SOURCE_DIR}/examples/crave_functionality"
"^${PROJECT_SOURCE_DIR}/logs"
"^${PROJECT_SOURCE_DIR}/tests"
"^${PROJECT_SOURCE_DIR}/crave.cfg"
"^${PROJECT_SOURCE_DIR}/temp*"
"^${PROJECT_SOURCE_DIR}/tmp*"
"^${PROJECT_SOURCE_DIR}/Makefile.local"
"^${PROJECT_SOURCE_DIR}/coverage.log"
"^${PROJECT_SOURCE_DIR}/lcov_output"
"~$"
)
include(CPack)




############################################################
############################################################
