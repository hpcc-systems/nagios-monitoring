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


# Component: hpcc nagios and nrpe config
#####################################################
# Description:
# ------------
#    CMake for hpcc nagios and nrpe config
#####################################################

IF ( "${packageManagement}" STREQUAL "RPM" )
        INSTALL ( FILES hpcc_nrpe.cfg DESTINATION /etc/nrpe.d/  )
        INSTALL ( FILES hpcc_commands.cfg DESTINATION /etc/nagios/conf.d/ )
ELSE ()
	INSTALL(FILES hpcc_nrpe.cfg DESTINATION   /etc/nagios/nrpe.d/)
	INSTALL(FILES hpcc_commands.cfg DESTINATION /etc/nagios3/conf.d/)
ENDIF ()
