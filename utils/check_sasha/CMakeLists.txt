################################################################################
#    HPCC SYSTEMS software Copyright (C) 2014 HPCC Systems.
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
################################################################################


# Component: check_sasha
#####################################################
# Description:
# ------------
#    Cmake check_sasha
#####################################################

project( check_sasha )

set (   SRCS
        main.cpp
    )

include_directories (
        ${HPCC_SOURCE_DIR}/common
        ${HPCC_SOURCE_DIR}/dali/sasha
        ${HPCC_SOURCE_DIR}/system/jlib
        ${HPCC_SOURCE_DIR}/system/include
        ${HPCC_SOURCE_DIR}/system/mp
        ${CMAKE_BINARY_DIR}/oss
        #${CMAKE_BINARY_DIR}
    )

ADD_DEFINITIONS( -D_CONSOLE )

HPCC_ADD_EXECUTABLE( check_sasha ${SRCS} )
install ( TARGETS check_sasha RUNTIME DESTINATION /usr/lib/nagios/plugins/ )
target_link_libraries ( check_sasha
         icudata
         jlib
         ws_workunits
         icui18n
         eclrtl

    )
