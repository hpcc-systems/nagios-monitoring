project (hpccsystems-nagios-monitoring)
cmake_minimum_required (VERSION 2.8.2)

set (HPCC_NM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (CMAKE_MODULE_PATH "${HPCC_NM_SOURCE_DIR}/cmake_modules/")

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

set (HPCC_SOURCE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

IF ("${HPCC_SOURCE_DIR}" STREQUAL "")
  SET (HPCC_SOURCE_DIR ${HPCC_SOURCE_ROOT_DIR}/HPCC-Platform)
ENDIF()

include (${HPCC_NM_SOURCE_DIR}/version.cmake)
include (${HPCC_NM_SOURCE_DIR}/cmake_modules/optionDefaults.cmake)
include (${HPCC_NM_SOURCE_DIR}/cmake_modules/nmCommonSetup.cmake)
include (${HPCC_SOURCE_DIR}/version.cmake)


SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fPIC -fmessage-length=0 -Wformat -Wformat-security -Wformat-nonliteral -pthread -Wuninitialized")

MESSAGE("*** START BUILDING HPCC PLATFORM ***")
MESSAGE (HPCC_SOURCE_DIR: ${HPCC_SOURCE_DIR})
ADD_SUBDIRECTORY(${HPCC_SOURCE_DIR} oss EXCLUDE_FROM_ALL)
CONFIGURE_FILE(${HPCC_SOURCE_DIR}/build-config.h.cmake "build-config.h" )
MESSAGE("*** END BUILDING HPCC PLATFORM ***")

#HPCC_ADD_SUBDIRECTORY (esp)
#HPCC_ADD_SUBDIRECTORY (initfiles)


SET(CPACK_PACKAGE_VERSION_MAJOR ${majorver})
SET(CPACK_PACKAGE_VERSION_MINOR ${minorver})
SET(CPACK_PACKAGE_VERSION_PATCH ${point}${stagever})


set (CPACK_PACKAGE_CONTACT "HPCCSystems <ossdevelopment@lexisnexis.com>")
set (CPACK_SOURCE_GENERATOR TGZ )
set (CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CPACK_RPM_PACKAGE_ARCHITECTURE}")

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
                SET ( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/postinst;${CMAKE_SOURCE_DIR}/prerm;${CMAKE_SOURCE_DIR}/postrm" )
            ENDIF ()

        ELSEIF ( ${packageManagement} STREQUAL "RPM" )
            SET ( CPACK_GENERATOR "${packageManagement}" )
            ###
            ## CPack instruction required for RPM
            ###
            MESSAGE("-- Will build RPM package")
            MESSAGE ("-- Packing BASH installation files")
            SET ( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/postinst" )
            SET ( CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURECE_DIR}/prerm" )
            SET ( CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/postrm" )
        ELSE()
            MESSAGE("WARNING: Unsupported package ${packageManagement}.")
        ENDIF ()

    ENDIF ( CMAKE_SYSTEM MATCHES Linux )
    IF ( EXISTS ${CMAKE_MODULE_PATH}/dependencies/${packageRevision}.cmake )
        INCLUDE( ${CMAKE_MODULE_PATH}/dependencies/${packageRevision}.cmake )
    ELSE()
         MESSAGE("-- WARNING: DEPENDENCY FILE FOR ${packageRevision} NOT FOUND")
    ENDIF()

    IF ( UNIX )
     SET ( CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )
    ENDIF()

    SET ( CPACK_MONOLITHIC_INSTALL TRUE )
		SET ( CPACK_PACKAGE_VENDOR "HPCC Systems" )
		INSTALL( FILES "${CMAKE_CURRENT_SOURCE_DIR}/modroxie.py" DESTINATION ${CMAKE_LIBRARY_PATH}/nagios/python_modules  )
		INSTALL( FILES "${CMAKE_CURRENT_SOURCE_DIR}/modroxie.pyconf" DESTINATION /etc/nagios/conf.d  )
		INCLUDE ( CPack )
ELSE()
    MESSAGE("WARNING: CMAKE 2.8.1 or later required to create RPMs from this project")
ENDIF()

