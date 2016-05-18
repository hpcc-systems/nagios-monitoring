##############################################################################
#
#    HPCC SYSTEMS software Copyright (C) 2012 HPCC Systems.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
##############################################################################

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set ( ARCH64BIT 1 )
else ()
  set ( ARCH64BIT 0 )
endif ()
message ("-- 64bit architecture is ${ARCH64BIT}")

  EXECUTE_PROCESS (
                COMMAND ${CMAKE_MODULE_PATH}/distrocheck.sh
                    OUTPUT_VARIABLE packageManagement
                        ERROR_VARIABLE  packageManagement
                )

IF ("${NM_COMMONSETUP_DONE}" STREQUAL "")
  SET (NM_COMMONSETUP_DONE 1)

  MACRO (MACRO_ENSURE_OUT_OF_SOURCE_BUILD _errorMessage)
    STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
    IF (insource)
      MESSAGE(FATAL_ERROR "${_errorMessage}")
    ENDIF(insource)
  ENDMACRO (MACRO_ENSURE_OUT_OF_SOURCE_BUILD)

  macro_ensure_out_of_source_build("nagios monitoring for HPCC  requires an out of source build.
    Please remove the directory ${CMAKE_BINARY_DIR}/CMakeFiles 
    and the file ${CMAKE_BINARY_DIR}/CMakeCache.txt,
    then create a separate build directory and run 'cmake path_to_source [options]' there.")

  cmake_policy ( SET CMP0011 NEW )

    set(CMAKE_MODULE_PATH "${HPCC_NM_SOURCE_DIR}/cmake_modules/")

  ##########################################################

  # common compiler/linker flags

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set ( CMAKE_BUILD_TYPE "Release" )
  elseif (NOT "${CMAKE_BUILD_TYPE}" MATCHES "Debug|Release")
    message (FATAL_ERROR "Unknown build type $ENV{CMAKE_BUILD_TYPE}")
  endif ()
  message ("-- Making ${CMAKE_BUILD_TYPE} system")

  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set ( ARCH64BIT 1 )
  else ()
    set ( ARCH64BIT 0 )
  endif ()
  message ("-- 64bit architecture is ${ARCH64BIT}")

  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

  set (CMAKE_THREAD_PREFER_PTHREAD 1)
  find_package(Threads)
  IF (NOT THREADS_FOUND)
    message(FATAL_ERROR "No threading support found")
  ENDIF()

  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
   set (CMAKE_COMPILER_IS_CLANGXX 1)
  endif()
  if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
   set (CMAKE_COMPILER_IS_CLANG 1)
  endif()
  if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_C_COMPILER_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif ()
  if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif ()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set ( CMAKE_BUILD_TYPE "Release" )
  elseif (NOT "${CMAKE_BUILD_TYPE}" MATCHES "Debug|Release")
    message (FATAL_ERROR "Unknown build type $ENV{CMAKE_BUILD_TYPE}")
  endif ()
  message ("-- Making ${CMAKE_BUILD_TYPE} system")

  # Build tag generation

  set(majorver ${HPCC_MAJOR})
  set(minorver ${HPCC_MINOR})
  set(point ${HPCC_POINT})
  if ( "${HPCC_MATURITY}" STREQUAL "release" )
    set(stagever "${HPCC_SEQUENCE}")
  else()
    set(stagever "${HPCC_MATURITY}${HPCC_SEQUENCE}")
  endif()
  set(version ${majorver}.${minorver}.${point})

  IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set( stagever "${stagever}Debug" )
  ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

 if (USE_OPTIONAL)
    message ("-- USE_OPTIONAL set - missing dependencies for optional features will automatically disable them")
  endif()

  if (NOT "${EXTERNALS_DIRECTORY}" STREQUAL "")
    message ("-- Using externals directory at ${EXTERNALS_DIRECTORY}")
  endif()

  IF ( NOT MAKE_DOCS_ONLY )
      IF ("${EXTERNALS_DIRECTORY}" STREQUAL "")
        SET(bisoncmd "bison")
        SET(flexcmd "flex")
      ELSE()
        IF (WIN32)
          SET(bisoncmdprefix "call")
          SET(flexcmdprefix "call")
          SET(bisoncmd "${EXTERNALS_DIRECTORY}\\bison\\bison.bat")
          SET(flexcmd "${EXTERNALS_DIRECTORY}\\bison\\flex.bat")
        ELSE ()
          SET(bisoncmd "${EXTERNALS_DIRECTORY}/bison/bison")
          SET(flexcmd "${EXTERNALS_DIRECTORY}/bison/flex")
        ENDIF()
      ENDIF()

      IF ("${BISON_VERSION}" STREQUAL "")
        IF (WIN32)
          # cmake bug workaround - it converts path separators fine in add_custom_command but not here
          STRING(REPLACE "/" "\\" BISON_exename "${bisoncmd}")
        ELSE()
          SET(BISON_exename "${bisoncmd}")
        ENDIF()
        EXECUTE_PROCESS(COMMAND ${BISON_exename} --version
          OUTPUT_VARIABLE BISON_version_output
          ERROR_VARIABLE BISON_version_error
          RESULT_VARIABLE BISON_version_result
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(REGEX REPLACE "^[^0-9]*([0-9.]+).*" "\\1" BISON_VERSION "${BISON_version_output}")
      ENDIF()

      IF ("${FLEX_VERSION}" STREQUAL "")
        IF (WIN32)
          # cmake bug workaround - it converts path separators fine in add_custom_command but not here
          STRING(REPLACE "/" "\\" FLEX_exename "${flexcmd}")
        ELSE()
          SET(FLEX_exename "${flexcmd}")
        ENDIF()
        EXECUTE_PROCESS(COMMAND ${FLEX_exename} --version
          OUTPUT_VARIABLE FLEX_version_output
          ERROR_VARIABLE FLEX_version_error
          RESULT_VARIABLE FLEX_version_result
          OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(REGEX REPLACE "^[^0-9]*([0-9.]+).*" "\\1" FLEX_VERSION "${FLEX_version_output}")
      ENDIF()

      IF ("${BISON_VERSION}" VERSION_LESS "2.4.1")
        MESSAGE(FATAL_ERROR "You need bison version 2.4.1 or later to build this project (version ${BISON_VERSION} detected)")
      ENDIF()

      IF ("${FLEX_VERSION}" VERSION_LESS "2.5.35")
        MESSAGE(FATAL_ERROR "You need flex version 2.5.35 or later to build this project (version ${FLEX_VERSION} detected)")
      ENDIF()

      IF (CMAKE_COMPILER_IS_GNUCXX)
        IF ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.1.1")
          MESSAGE(FATAL_ERROR "You need Gnu c++ version 4.1.1 or later to build this project (version ${CMAKE_CXX_COMPILER_VERSION} detected)")
        ENDIF()
      ENDIF()
    ENDIF()

      if(USE_XALAN)
        find_package(XALAN)
        if (XALAN_FOUND)
          add_definitions (-D_USE_XALAN)
        else()
          message(FATAL_ERROR "XALAN requested but package not found")
        endif()
      endif(USE_XALAN)

      if(USE_LIBXSLT)
        find_package(LIBXSLT)
        if (LIBXSLT_FOUND)
          add_definitions (-D_USE_LIBSLT)
        else()
          message(FATAL_ERROR "LIBXSLT requested but package not found")
        endif()
      endif(USE_LIBXSLT)

      if(USE_XERCES)
        find_package(XERCES)
        if (XERCES_FOUND)
          add_definitions (-D_USE_XERCES)
        else()
          message(FATAL_ERROR "XERCES requested but package not found")
        endif()
      endif(USE_XERCES)

      if(USE_LIBXML2)
        find_package(LIBXML2)
        if (LIBXML2_FOUND)
          add_definitions (-D_USE_LIBXML2)
        else()
          message(FATAL_ERROR "LIBXML2 requested but package not found")
        endif()
      endif(USE_LIBXML2)



  ###########################################################################
  # This macro allows for disabling a directory based on the value of a variable passed to the macro.
  #
  # ex. HPCC_ADD_SUBDIRECORY(roxie ${CLIENTTOOLS_ONLY})
  #
  # This call will disable the roxie dir if -DCLIENTTOOLS_ONLY=ON is set at config time.
  #
  macro(HPCC_ADD_SUBDIRECTORY)
    set(adddir OFF)
    PARSE_ARGUMENTS(_HPCC_SUB "" "" ${ARGN})
    LIST(GET _HPCC_SUB_DEFAULT_ARGS 0 subdir)
    set(flags ${_HPCC_SUB_DEFAULT_ARGS})
    LIST(REMOVE_AT flags 0)
    LIST(LENGTH flags length)
    if(NOT length)
      set(adddir ON)
    else()
      foreach(f ${flags})
        if(${f})
          set(adddir ON)
        endif()
      endforeach()
    endif()
    if ( adddir )
      add_subdirectory(${subdir})
    endif()
  endmacro(HPCC_ADD_SUBDIRECTORY)

  set ( SCM_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated )
  include_directories (${SCM_GENERATED_DIR})

  macro(ADD_PLUGIN)
    PARSE_ARGUMENTS(PLUGIN
        "PACKAGES;OPTION;MINVERSION;MAXVERSION"
        ""
        ${ARGN}
    )
    LIST(GET PLUGIN_DEFAULT_ARGS 0 PLUGIN_NAME)
    string(TOUPPER ${PLUGIN_NAME} name)
    set(PLUGIN_FOUND 0)
    set(PLUGIN_MDEPS ${PLUGIN_NAME}_mdeps)
    set(${PLUGIN_MDEPS} "")

    FOREACH(package ${PLUGIN_PACKAGES})
      set(findvar ${package}_FOUND)
      string(TOUPPER ${findvar} PACKAGE_FOUND)
      if ("${PLUGIN_MINVERSION}" STREQUAL "")
        find_package(${package})
      else()
        set(findvar ${package}_VERSION_STRING)
        string(TOUPPER ${findvar} PACKAGE_VERSION_STRING)
        find_package(${package} ${PLUGIN_MINVERSION} )
        if ("${${PACKAGE_VERSION_STRING}}" VERSION_GREATER "${PLUGIN_MAXVERSION}")
          set(${PLUGIN_FOUND} 0)
        endif()
      endif()
      if (${PACKAGE_FOUND})
        set(PLUGIN_FOUND 1)
      else()
        set(PLUGIN_FOUND 0)
        set(${PLUGIN_MDEPS} ${${PLUGIN_MDEPS}} ${package})
      endif()
    ENDFOREACH()
    option(${PLUGIN_OPTION} "Turn on optional plugin based on availability of dependencies" ${PLUGIN_FOUND})
    LOG_PLUGIN(${PLUGIN_NAME} OPTION ${PLUGIN_OPTION} MDEPS ${${PLUGIN_MDEPS}})
    if(${PLUGIN_FOUND})
      set(bPLUGINS ${bPLUGINS} ${PLUGIN_NAME})
    else()
      set(nbPLUGINS ${nbPLUGINS} ${PLUGIN_NAME})
    endif()
  endmacro()

  macro(LOG_PLUGIN)
    PARSE_ARGUMENTS(pLOG
      "OPTION;MDEPS"
      ""
      ${ARGN}
    )
    LIST(GET pLOG_DEFAULT_ARGS 0 PLUGIN_NAME)
    if ( ${pLOG_OPTION} )
        message(STATUS "Building Plugin: ${PLUGIN_NAME}" )
    else()
      message("---- WARNING -- Not Building Plugin: ${PLUGIN_NAME}")
      foreach (dep ${pLOG_MDEPS})
        MESSAGE("---- WARNING -- Missing dependency: ${dep}")
      endforeach()
      if (NOT USE_OPTIONAL)
        message(FATAL_ERROR "Optional dependencies missing and USE_OPTIONAL not set")
      endif()
    endif()
  endmacro()

  macro(LOG_PLUGIN)
    PARSE_ARGUMENTS(pLOG
      "OPTION;MDEPS"
      ""
      ${ARGN}
    )
    LIST(GET pLOG_DEFAULT_ARGS 0 PLUGIN_NAME)
    if ( ${pLOG_OPTION} )
        message(STATUS "Building Plugin: ${PLUGIN_NAME}" )
    else()
      message("---- WARNING -- Not Building Plugin: ${PLUGIN_NAME}")
      foreach (dep ${pLOG_MDEPS})
        MESSAGE("---- WARNING -- Missing dependency: ${dep}")
      endforeach()
      if (NOT USE_OPTIONAL)
        message(FATAL_ERROR "Optional dependencies missing and USE_OPTIONAL not set")
      endif()
    endif()
  endmacro()

  # This Macro is provided as Public domain from
  # http://www.cmake.org/Wiki/CMakeMacroParseArguments
  MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
    SET(DEFAULT_ARGS)
    FOREACH(arg_name ${arg_names})
      SET(${prefix}_${arg_name})
    ENDFOREACH(arg_name)
    FOREACH(option ${option_names})
      SET(${prefix}_${option} FALSE)
    ENDFOREACH(option)

    SET(current_arg_name DEFAULT_ARGS)
    SET(current_arg_list)
    FOREACH(arg ${ARGN})
      SET(larg_names ${arg_names})
      LIST(FIND larg_names "${arg}" is_arg_name)
      IF (is_arg_name GREATER -1)
        SET(${prefix}_${current_arg_name} ${current_arg_list})
        SET(current_arg_name ${arg})
        SET(current_arg_list)
      ELSE (is_arg_name GREATER -1)
        SET(loption_names ${option_names})
        LIST(FIND loption_names "${arg}" is_option)
        IF (is_option GREATER -1)
          SET(${prefix}_${arg} TRUE)
        ELSE (is_option GREATER -1)
          SET(current_arg_list ${current_arg_list} ${arg})
        ENDIF (is_option GREATER -1)
      ENDIF (is_arg_name GREATER -1)
    ENDFOREACH(arg)
    SET(${prefix}_${current_arg_name} ${current_arg_list})
  ENDMACRO(PARSE_ARGUMENTS)

  macro(HPCC_ADD_EXECUTABLE target)
    add_executable(${target} ${ARGN})
  endmacro(HPCC_ADD_EXECUTABLE target)

  macro(HPCC_ADD_LIBRARY target)
    add_library(${target} ${ARGN})
  endmacro(HPCC_ADD_LIBRARY target)

  set ( CMAKE_INSTALL_PREFIX "${PREFIX}/${DIR_NAME}" )

  set (CMAKE_SKIP_BUILD_RPATH  FALSE)
  set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_DIR}")
  set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  MACRO (FETCH_GIT_TAG workdir edition result)
      execute_process(COMMAND "${GIT_COMMAND}" describe --tags --dirty --abbrev=6 --match ${edition}*
        WORKING_DIRECTORY "${workdir}"
        OUTPUT_VARIABLE ${result}
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
        if ("${${result}}" STREQUAL "")
            execute_process(COMMAND "${GIT_COMMAND}" describe --always --tags --all --abbrev=6 --dirty --long
                WORKING_DIRECTORY "${workdir}"
                OUTPUT_VARIABLE ${result}
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
  ENDMACRO()

  function(LIST_TO_STRING separator outvar)
    set ( tmp_str "" )
    list (LENGTH ARGN list_length)
    if ( ${list_length} LESS 2 )
      set ( tmp_str "${ARGN}" )
    else()
      math(EXPR last_index "${list_length} - 1")

      foreach( index RANGE ${last_index} )
        if ( ${index} GREATER 0 )
          list( GET ARGN ${index} element )
          set( tmp_str "${tmp_str}${separator}${element}")
        else()
          list( GET ARGN 0 element )
          set( tmp_str "${element}")
        endif()
      endforeach()
    endif()
    set ( ${outvar} "${tmp_str}" PARENT_SCOPE )
  endfunction()

  function(STRING_TO_LIST separator outvar stringvar)
    set( tmp_list "" )
    string(REPLACE "${separator}" ";" tmp_list ${stringvar})
    string(STRIP "${tmp_list}" tmp_list)
    set( ${outvar} "${tmp_list}" PARENT_SCOPE)
  endfunction()



  ###########################################################################
  ###
  ## The following sets the dependency list for a package
  ###
  ###########################################################################
  function(SET_DEPENDENCIES cpackvar)
    set(_tmp "")
    if(${cpackvar})
      STRING_TO_LIST(", " _tmp ${${cpackvar}})
    endif()
    foreach(element ${ARGN})
      list(APPEND _tmp ${element})
    endforeach()
    list(REMOVE_DUPLICATES _tmp)
    LIST_TO_STRING(", " _tmp "${_tmp}")
    set(${cpackvar} "${_tmp}" CACHE STRING "" FORCE)
    message(STATUS "Updated ${cpackvar} to ${${cpackvar}}")
  endfunction()

endif()
