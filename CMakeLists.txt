project (hpccsystems-nagios-monitoring)
cmake_minimum_required (VERSION 2.8.2)

set (HPCC_NM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE ("************ Setting CMAKE_MODULE_PATH to ${HPCC_NM_SOURCE_DIR}/cmake_modules/")
set (CMAKE_MODULE_PATH "${HPCC_NM_SOURCE_DIR}/cmake_modules/")
MESSAGE ("CMAKE_MODULE_PATH is set to ${CMAKE_MODULE_PATH}")

###
## Build Level
###
if( NOT BUILD_LEVEL )
    set ( BUILD_LEVEL "COMMUNITY" )
endif()
###

option(EXEC_PREFIX "Set the execution prefix")

if (NOT LICENSE_FILE)
    set (LICENSE_FILE "LICENSE.txt")
endif()

MESSAGE ( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )


set (HPCC_SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

IF ("${HPCC_SOURCE_DIR}" STREQUAL "")
  SET (HPCC_SOURCE_DIR ${HPCC_SOURCE_ROOT_DIR}/HPCC-Platform)
ENDIF()

#MESSAGE ( "32 - CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )


include (${HPCC_NM_SOURCE_DIR}/version.cmake)
include (${HPCC_SOURCE_DIR}/version.cmake)
include (${HPCC_NM_SOURCE_DIR}/cmake_modules/optionDefaults.cmake)
include (${HPCC_NM_SOURCE_DIR}/cmake_modules/nmCommonSetup.cmake)
#include (${HPCC_SOURCE_DIR}/cmake_modules/commonSetup.cmake)
include (${HPCC_SOURCE_DIR}/version.cmake)

#MESSAGE ( "40 - CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )

MESSAGE ( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fPIC -fmessage-length=0 -Wformat -Wformat-security -Wformat-nonliteral -pthread -Wuninitialized")

MESSAGE("*** START BUILDING HPCC PLATFORM ***")
MESSAGE (HPCC_SOURCE_DIR: ${HPCC_SOURCE_DIR})
ADD_SUBDIRECTORY(${HPCC_SOURCE_DIR} oss EXCLUDE_FROM_ALL)
INCLUDE(${HPCC_NM_SOURCE_DIR}/platform-version-prereq.cmake)

###
## Run file configuration to set build tag along with install lines for generated
## config files.
###
set( BUILD_TAG "${projname}_${version}-${stagever}")
if (USE_GIT_DESCRIBE OR CHECK_GIT_TAG)
    FETCH_GIT_TAG (${CMAKE_SOURCE_DIR} ${projname}_${version} GIT_BUILD_TAG)
    message ("-- Git tag is '${GIT_BUILD_TAG}'")
    if (NOT "${GIT_BUILD_TAG}" STREQUAL "${BUILD_TAG}")
        if (CHECK_GIT_TAG)
            message(FATAL_ERROR "Git tag '${GIT_BUILD_TAG}' does not match source version '${BUILD_TAG}'" )
        else()
            if(NOT "${GIT_BUILD_TAG}" STREQUAL "") # probably means being built from a tarball...
                set( BUILD_TAG "${BUILD_TAG}[${GIT_BUILD_TAG}]")
            endif()
        endif()
    endif()
endif()
message ("-- Build tag is '${BUILD_TAG}'")
if (NOT "${BASE_BUILD_TAG}" STREQUAL "")
    set(BASE_BUILD_TAG "${BUILD_TAG}")
endif()
message ("-- Base build tag is '${BASE_BUILD_TAG}'")
CONFIGURE_FILE(${HPCC_SOURCE_DIR}/build-config.h.cmake "build-config.h" )
MESSAGE("*** END BUILDING HPCC PLATFORM ***")

MESSAGE ( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )

HPCC_ADD_SUBDIRECTORY( hpcc-nagios-tools )
HPCC_ADD_SUBDIRECTORY( utils )
HPCC_ADD_SUBDIRECTORY( config )


SET(CPACK_PACKAGE_VERSION_MAJOR ${majorver})
SET(CPACK_PACKAGE_VERSION_MINOR ${minorver})
SET(CPACK_PACKAGE_VERSION_PATCH ${point}${stagever})


set (CPACK_PACKAGE_CONTACT "HPCCSystems <ossdevelopment@lexisnexis.com>")
set (CPACK_SOURCE_GENERATOR TGZ )
if ( ${ARCH64BIT} EQUAL 1 )
    set ( CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
else( ${ARCH64BIT} EQUAL 1 )
    set ( CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
endif ( ${ARCH64BIT} EQUAL 1 )

set (CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CPACK_RPM_PACKAGE_ARCHITECTURE}")

MESSAGE ( "CPACK_RPM_PACKAGE_ARCHITECTURE ${CPACK_RPM_PACKAGE_ARCHITECTURE}" )
MESSAGE ( "CPACK_SYSTEM_NAME ${CPACK_SYSTEM_NAME}" )
MESSAGE ( "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}" )
MESSAGE ( "${CMAKE_MODULE_PATH}/distrocheck.sh" )
MESSAGE ( "HPCC_NM_SOURCE_DIR ${HPCC_NM_SOURCE_DIR}" )



IF ( CMAKE_SYSTEM_NAME MATCHES Linux )
    EXECUTE_PROCESS (
                COMMAND ${CMAKE_MODULE_PATH}/distrocheck.sh
                    OUTPUT_VARIABLE packageManagement
                        ERROR_VARIABLE  packageManagement
                )
    EXECUTE_PROCESS (
                COMMAND ${CMAKE_MODULE_PATH}/getpackagerevisionarch.sh
                    OUTPUT_VARIABLE packageRevisionArch
                        ERROR_VARIABLE  packageRevisionArch
                )
    EXECUTE_PROCESS (
                COMMAND ${CMAKE_MODULE_PATH}/getpackagerevisionarch.sh --noarch
                    OUTPUT_VARIABLE packageRevision
                        ERROR_VARIABLE  packageRevision
                )

    MESSAGE ( "-- Auto Detecting Packaging type")
    MESSAGE ( "-- distro uses ${packageManagement}, revision is ${packageRevisionArch}" )

    IF ( ${packageManagement} STREQUAL "DEB" )
        SET(CPACK_PACKAGE_FILE_NAME     "${CMAKE_PROJECT_NAME}-${version}-${stagever}${packageRevisionArch}")
        SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib)
    ELSEIF ( ${packageManagement} STREQUAL "RPM" )
        SET(CPACK_PACKAGE_FILE_NAME     "${CMAKE_PROJECT_NAME}-${version}-${stagever}.${packageRevisionArch}")
        SET(CMAKE_LIBRARY_PATH "/usr/lib64")
    ELSE()
        SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${version}-${stagever}${CPACK_SYSTEM_NAME}")
    ENDIF ()

ENDIF ( CMAKE_SYSTEM_NAME MATCHES Linux )

#MESSAGE ("-- Current release version is ${CPACK_PACKAGE_FILE_NAME}")
#MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
#MESSAGE( STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR} )
#MESSAGE( STATUS "CMAKE_LIBRARY_PATH: "${CMAKE_LIBRARY_PATH} )

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS) 

if ("${LIB64}" STREQUAL "TRUE") 
    set(LIBSUFFIX 64) 
else() 
    set(LIBSUFFIX "") 
endif() 

set(INSTALL_LIB_DIR  lib${LIBSUFFIX} CACHE PATH "Installation directory for libraries") 
mark_as_advanced(INSTALL_LIB_DIR) 
#message( STATUS "INSTALL_LIB_DIR: ${INSTALL_LIB_DIR}" )

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS) 
#message( STATUS "lib64: ${LIB64}" )

SET ( CPACK_SOURCE_GENERATOR TGZ )

IF (NOT "${CMAKE_VERSION}" VERSION_LESS "2.8.2")
    IF ( CMAKE_SYSTEM MATCHES Linux )
        IF ( ${packageManagement} STREQUAL "DEB" )
            IF ("${CMAKE_VERSION}" VERSION_EQUAL "2.8.2")
                MESSAGE("WARNING: CMAKE 2.8.2  would not build DEB package")
            ELSE ()
                SET ( CPACK_GENERATOR "${packageManagement}" )
                MESSAGE("-- Will build DEB package")
                ###
                ## CPack instruction required for Debian
                ###
                MESSAGE ("-- Packing BASH installation files")
            ENDIF ()

        ELSEIF ( ${packageManagement} STREQUAL "RPM" )
            SET ( CPACK_GENERATOR "${packageManagement}" )
            ###
            ## CPack instruction required for RPM
            ###
            MESSAGE("-- Will build RPM package")
            MESSAGE ("-- Packing BASH installation files")
        ELSE()
            MESSAGE("WARNING: Unsupported package ${packageManagement}.")
        ENDIF ()

    ENDIF ( CMAKE_SYSTEM MATCHES Linux )
    IF ( EXISTS ${CMAKE_MODULE_PATH}/dependencies/${packageRevision}.cmake )
	CONFIGURE_FILE(${HPCC_NM_SOURCE_DIR}/cmake_modules/dependencies/${packageRevision}.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/dependencies/${packageRevision}.cmake)
        INCLUDE( ${CMAKE_MODULE_PATH}/dependencies/${packageRevision}.cmake )
    ELSE()
         MESSAGE("-- WARNING: DEPENDENCY FILE FOR ${packageRevision} NOT FOUND")
    ENDIF()

    IF ( UNIX )
     SET ( CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )
    ENDIF()

    SET ( CPACK_MONOLITHIC_INSTALL TRUE )
		SET ( CPACK_PACKAGE_VENDOR "HPCC Systems" )
#		INSTALL( FILES "${CMAKE_CURRENT_SOURCE_DIR}/modroxie.py" DESTINATION ${CMAKE_LIBRARY_PATH}/nagios/python_modules  )
#		INSTALL( FILES "${CMAKE_CURRENT_SOURCE_DIR}/modroxie.pyconf" DESTINATION /etc/nagios/conf.d  )
		INCLUDE ( CPack )
ELSE()
    MESSAGE("WARNING: CMAKE 2.8.1 or later required to create RPMs from this project")
ENDIF()

