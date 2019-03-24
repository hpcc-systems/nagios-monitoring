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


# Component: check_dafilesrv
#####################################################
# Description:
# ------------
#    Cmake check_dafilesrv
#####################################################

project( check_dafilesrv )

set (   SRCS
        main.cpp
    )

include_directories (
        ${HPCC_SOURCE_DIR}/common
	${HPCC_SOURCE_DIR}/common/remote
        ${HPCC_SOURCE_DIR}/system/jlib
        ${HPCC_SOURCE_DIR}/system/include
        ${HPCC_SOURCE_DIR}/fs/dafsclient
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/oss
    )

ADD_DEFINITIONS( -D_CONSOLE )

HPCC_ADD_EXECUTABLE( check_dafilesrv ${SRCS} )

IF ( "${packageManagement}" STREQUAL "RPM" )
        IF ( "${ARCH64BIT}" EQUAL 1 )
		install ( TARGETS check_dafilesrv RUNTIME DESTINATION /usr/lib64/nagios/plugins/ )
	ELSE ()
		install ( TARGETS check_dafilesrv RUNTIME DESTINATION /usr/lib//nagios/plugins/ )
	ENDIF ()
ELSE ()
	install ( TARGETS check_dafilesrv RUNTIME DESTINATION /usr/lib/nagios/plugins/ )
ENDIF ()

target_link_libraries ( check_dafilesrv
         jlib
         dalibase
    )
