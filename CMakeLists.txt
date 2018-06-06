project (hpccsystems-nagios-monitoring)
cmake_minimum_required (VERSION 3.5.1)

set(HPCC_NM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${HPCC_NM_SOURCE_DIR}/cmake_modules/")
message(STATUS "CMAKE_MODULE_PATH is set to ${CMAKE_MODULE_PATH}")

###
## Build Level
###
if(NOT BUILD_LEVEL)
    set(BUILD_LEVEL "COMMUNITY")
endif()
###

set(BUILD_TAG "${projname}_${version}-${stagever}")

set(HPCC_SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

if("${HPCC_SOURCE_DIR}" STREQUAL "")
    set(HPCC_SOURCE_DIR ${HPCC_SOURCE_ROOT_DIR}/HPCC-Platform)
endif()

include(${HPCC_NM_SOURCE_DIR}/version.cmake)
include(${HPCC_SOURCE_DIR}/cmake_modules/optionDefaults.cmake)
include(${CMAKE_MODULE_PATH}/nmCommonSetup.cmake)
include(${HPCC_NM_SOURCE_DIR}/platform-version-prereq.cmake)

set(BUILD_TAG "${projname}_${version}-${stagever}")

message(STATUS "HPCC_SOURCE_DIR: ${HPCC_SOURCE_DIR}")
add_subdirectory(${HPCC_SOURCE_DIR} oss EXCLUDE_FROM_ALL)

configure_file(${HPCC_SOURCE_DIR}/build-config.h.cmake "build-config.h")

HPCC_ADD_SUBDIRECTORY(hpcc-nagios-tools)
HPCC_ADD_SUBDIRECTORY(utils)
HPCC_ADD_SUBDIRECTORY(config)
HPCC_ADD_SUBDIRECTORY(commands)

message(STATUS "HPCC_SOURCE_DIR: ${HPCC_SOURCE_DIR}")
message(STATUS "HPCC_NM_SOURCE_DIR ${HPCC_NM_SOURCE_DIR}")

set(CPACK_PACKAGE_CONTACT "HPCCSystems <ossdevelopment@lexisnexis.com>")
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CPACK_RPM_PACKAGE_ARCHITECTURE}")
set(CPACK_PACKAGE_VERSION_MAJOR ${majorver})
set(CPACK_PACKAGE_VERSION_MINOR ${minorver})
set(CPACK_PACKAGE_VERSION_PATCH ${point}-${stagever})
set(CPACK_RPM_PACKAGE_VERSION "${version}" )
set(CPACK_RPM_PACKAGE_RELEASE "${stagever}")

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    execute_process(
        COMMAND ${CMAKE_MODULE_PATH}/distrocheck.sh
        OUTPUT_VARIABLE packageManagement
        ERROR_VARIABLE  packageManagement)
    execute_process(
        COMMAND ${CMAKE_MODULE_PATH}/getpackagerevisionarch.sh
        OUTPUT_VARIABLE packageRevisionArch
        ERROR_VARIABLE  packageRevisionArch)
    execute_process(
        COMMAND ${CMAKE_MODULE_PATH}/getpackagerevisionarch.sh --noarch
        OUTPUT_VARIABLE packageRevision
        ERROR_VARIABLE  packageRevision)

    message(STATUS "Auto Detecting Packaging type")
    message(STATUS "distro uses ${packageManagement}, revision is ${packageRevisionArch}")

    if(${packageManagement} STREQUAL "DEB")
        set(CPACK_PACKAGE_FILE_NAME     "${CMAKE_PROJECT_NAME}-${version}-${stagever}${packageRevisionArch}")
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib)
    elseif(${packageManagement} STREQUAL "RPM")
        set(CPACK_PACKAGE_FILE_NAME     "${CMAKE_PROJECT_NAME}-${version}-${stagever}.${packageRevisionArch}")
        set(CMAKE_LIBRARY_PATH "/usr/lib64")
    else()
        set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${version}-${stagever}${CPACK_SYSTEM_NAME}")
    endif()

endif(CMAKE_SYSTEM_NAME MATCHES Linux)

#message(STATUS "Current release version is ${CPACK_PACKAGE_FILE_NAME}")
#message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
#message(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
#message(STATUS "CMAKE_LIBRARY_PATH: "${CMAKE_LIBRARY_PATH})

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS) 

if ("${LIB64}" STREQUAL "TRUE") 
    set(LIBSUFFIX 64) 
else() 
    set(LIBSUFFIX "") 
endif() 

set(INSTALL_LIB_DIR  lib${LIBSUFFIX} CACHE PATH "Installation directory for libraries") 
mark_as_advanced(INSTALL_LIB_DIR) 
#message(STATUS "INSTALL_LIB_DIR: ${INSTALL_LIB_DIR}")

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS) 
#message(STATUS "lib64: ${LIB64}")

set(CPACK_SOURCE_GENERATOR TGZ)

if(CMAKE_SYSTEM MATCHES Linux)

    ## setup dependency information
    if("${HPCC_MATURITY_REQ}" STREQUAL "release")
        set(HPCC_STAGEVER_REQ "${HPCC_SEQUENCE_REQ}")
    else()
        set(HPCC_STAGEVER_REQ "${HPCC_MATURITY_REQ}${HPCC_SEQUENCE_REQ}")
    endif()
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(HPCC_STAGEVER_REQ "${HPCC_STAGEVER_REQ}Debug")
    endif()
    set(VER_SEPARATOR "-")
    if("${HPCC_MATURITY_REQ}" MATCHES "^rc[0-9]+$")
        set(VER_SEPARATOR "~")
    endif()
    set(DEBIAN_REQUIRED_PLATFORM_PACKAGE ${HPCC_MAJOR_REQ}.${HPCC_MINOR_REQ}.${HPCC_POINT_REQ}${VER_SEPARATOR}${HPCC_STAGEVER_REQ})
    set(REDHAT_REQUIRED_PLATFORM_PACKAGE ${HPCC_MAJOR_REQ}.${HPCC_MINOR_REQ}.${HPCC_POINT_REQ})
    if(${packageManagement} STREQUAL "DEB")
        set(CPACK_GENERATOR "${packageManagement}")
        message("-- Will build DEB package")
        ###
        ## CPack instruction required for Debian
        ###
        message("-- Packing BASH installation files")
        set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_BINARY_DIR}/postinst;${CMAKE_SOURCE_DIR}/prerm;${CMAKE_BINARY_DIR}/postrm")
        SET_DEPENDENCIES(CPACK_DEBIAN_PACKAGE_DEPENDS "hpccsystems-platform (>= ${DEBIAN_REQUIRED_PLATFORM_PACKAGE})")

    elseif(${packageManagement} STREQUAL "RPM")
        set(CPACK_GENERATOR "${packageManagement}")
        ###
        ## CPack instruction required for RPM
        ###
        message("-- Will build RPM package")
        message("-- Packing BASH installation files")
        set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_BINARY_DIR}/postinst")
        set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/prerm")
        set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_BINARY_DIR}/postrm")
        SET_DEPENDENCIES(CPACK_RPM_PACKAGE_REQUIRES "hpccsystems-platform >= ${REDHAT_REQUIRED_PLATFORM_PACKAGE}")
    else()
        message("WARNING: Unsupported package ${packageManagement}.")
    endif()

endif(CMAKE_SYSTEM MATCHES Linux)
if(EXISTS ${CMAKE_MODULE_PATH}/dependencies/${packageRevision}.cmake)
    include(${CMAKE_MODULE_PATH}/dependencies/${packageRevision}.cmake)
else()
    message(WARNING "DEPENDENCY FILE FOR ${packageRevision} NOT FOUND")
endif()

if(UNIX)
    set(CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_PACKAGE_VendOR "HPCC Systems")
include(CPack)
