/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2015 HPCC Systems.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#include "HPCCNagiosToolSet.hpp"
#include "jstring.hpp"
#include <iostream>

void usage()
{
    std::cout << "HPCC Systems Nagios configuration file generator (version: " << BUILD_VERSION_MAJOR  << "." << BUILD_VERSION_MINOR << "." << BUILD_VERSION_POINT << ")\n\
Usage: hpcc-nagios-tools -env <environment file> -out <output path> [options]\n\
Usage: hpcc-nagios-tools -env /tmp/env190.xml -u \"\\$USER3\\$\" -p \"\\$USER4\\$\" -o /etc/nagios3/conf.d/hpcc1services.cfg -s -usernumwarn 10  -usernumcrit 15 -disable_check_all_disks\n\n";
    std::cout << "  -c or -cfggen <val>         : The path to the configgen.  (Default: /opt/HPCCSystems/sbin/configgen)\n";
    std::cout << "  -g or -hostgroup            : generate host group file\n";
    std::cout << "  -s or -service              : generate service and host file\n";
    std::cout << "  -n or -nrpe                 : generate client plugin cfgs for nrpe\n";
    std::cout << "  -e or -env <val>            : hpcc environment configuration file (Default: /etc/HPCCSystems/environment.xml)\n";
    std::cout << "  -ec or -escalation_cmds     : generate escalation commands\n";
    std::cout << "  -enable_host_notify         : enable host notifications (default: " << CHPCCNagiosToolSet::m_bEnableHostEscalations << ")\n";
    std::cout << "  -enable_service_notify      : enable service notifications (default: " << CHPCCNagiosToolSet::m_bEnableServiceEscalations << ")\n";
    std::cout << "  -set_url <val>              : set the url link for escalation notifications (Default " << CHPCCNagiosToolSet::m_pNotificationURL << "\n";
    std::cout << "  -override_send_serivce_status <val> : override send_status escalation command (default: " << CHPCCNagiosToolSet::m_pSendServiceStatus << ")\n";
    std::cout << "  -override_send_host_status <val>    : override send_status escalation command (default: " << CHPCCNagiosToolSet::m_pSendHostStatus << ")\n";
    std::cout << "  -override_service_status <val>: override host_notification_commands (default: " << CHPCCNagiosToolSet::m_pServiceNotificatonCommand << ")\n";
    std::cout << "  -override_host_status <val>   : override service_notification_commands (default: " << CHPCCNagiosToolSet::m_pHostNotificatonCommand << ")\n";
    std::cout << "  -override_eclwatch_host_port: override eclwatch host port for escalation commands.  (May be specified multiple times)\n";
    std::cout << "  -o or -output <val>         : outpfile where the generated configuration will be written\n";
    std::cout << "  -r or -retry                : keep attempting to resolve IP to hostnames (Default: will stop resolution after 1st failure)\n";
    std::cout << "  -u or -user <val>           : MACRO name to use for username for esp server login (e.g. $USER1$)\n";
    std::cout << "  -p or -pass <val>           : MACRO to use for password for esp server login (e.g. $USER2$)\n";
    std::cout << "  -attempts <val>             : max host retry attempts   (defaut: " <<  CHPCCNagiosToolSet::m_uMaxCheckAttempts << ")\n";
    std::cout << "  -sysload1warn <val>         : load1 warning threshold   (Default: " << CHPCCNagiosToolSet::m_fSystemLoad1Warn << ")\n";
    std::cout << "  -sysload5warn <val>         : load5 warning threshold   (Default: " << CHPCCNagiosToolSet::m_fSystemLoad5Warn << ")\n";
    std::cout << "  -sysload15warn <val>        : load15 warning threshold  (Default: " << CHPCCNagiosToolSet::m_fSystemLoad15Warn  << ")\n";
    std::cout << "  -sysload1crit <val>         : load1 critical threshold  (Default: " << CHPCCNagiosToolSet::m_fSystemLoad1Critical << ")\n";
    std::cout << "  -sysload5crit <val>         : load5 critical threshold  (Default: " << CHPCCNagiosToolSet::m_fSystemLoad5Critical << ")\n";
    std::cout << "  -sysload15crit <val>        : load15 critical threshold (Default: " << CHPCCNagiosToolSet::m_fSystemLoad15Critical << ")\n";
    std::cout << "  -diskspacewarn <val>        : disk space % warning threshold  (Default: " << CHPCCNagiosToolSet::m_nDiskSpacePercentageWarning  << ")\n";
    std::cout << "  -diskspacecrit <val>        : disk space % critical threshold (Default: " << CHPCCNagiosToolSet::m_nDiskSpacePercentageCritical << ")\n";
    std::cout << "  -usernumwarn <val>          : users logged in warning threshold  (Default: " << CHPCCNagiosToolSet::m_nUserNumberWarning << ")\n";
    std::cout << "  -usernumcrit <val>          : users logged in critical threshold (Default: " << CHPCCNagiosToolSet::m_nUserNumberCritical << ")\n";
    std::cout << "  -totalprocswarn <val>       : total process warning threshold   (Default: " << CHPCCNagiosToolSet::m_nTotalProcsWarning << ")\n";
    std::cout << "  -totalprocscrit <val>       : total process critical threshold  (Default: " << CHPCCNagiosToolSet::m_nTotalProcsCritical << ")\n";
    std::cout << "  -checkperiod <val>          : host check period (Default: " << CHPCCNagiosToolSet::m_pCheckPeriod << ")\n";
    std::cout << "  -contacts <val>             : host contacts     (Default: " << CHPCCNagiosToolSet::m_pContacts << ")\n";
    std::cout << "  -contactgroups <val>        : host contact groups (Default: " << CHPCCNagiosToolSet::m_pContactGroups << ")\n";
    std::cout << "  -notify_interval <val>      : set notification interval (Default: " << CHPCCNagiosToolSet::m_nNotificationInterval << ")\n";
    std::cout << "  -notify_period <val>        : set notification period (Default: " << CHPCCNagiosToolSet::m_pNotificationPeriod << ")\n";
    std::cout << "  -set_esp_username_pw <esp name> <username> <password> : set specific logins credentials for esp checks.  All fields are required. Can be specified more than 1x\n";
    std::cout << "  -override_check_all_disks <val> : check_all_disk plugin name (Default: " << CHPCCNagiosToolSet::m_pCheckDiskSpace << ")\n";
    std::cout << "  -override_check_users <val>     : check_users plugin name (Default: " << CHPCCNagiosToolSet::m_pCheckUsers << ")\n";
    std::cout << "  -override_check_procs <val>     : check_procs plugin name (Default: " << CHPCCNagiosToolSet::m_pCheckProcs << ")\n";
    std::cout << "  -override_check_load <val>      : check_load plugin name  (Default: " << CHPCCNagiosToolSet::m_pCheckLoad << ")\n";
    std::cout << "  -override_retry_interval <val>          : check retry_interval (Default: " << CHPCCNagiosToolSet::m_nRetryInteval << ")\n";
    std::cout << "  -override_active_checks_enabled <val>   : active_checks (Default: " << CHPCCNagiosToolSet::m_nActiveChecksEnabled << ")\n";
    std::cout << "  -override_passive_checks_enabled <val>  : passive_checks (Default: " << CHPCCNagiosToolSet::m_nPassiveChecksEnabled << ")\n";
    std::cout << "  -override_parallelize_check <val>       : parallelize_check (Default: " << CHPCCNagiosToolSet::m_nParallelizeCheck << ")\n";
    std::cout << "  -override_obsess_over_service <val>     : obsess_over_service (Default: " << CHPCCNagiosToolSet::m_nObsessOverService << ")\n";
    std::cout << "  -override_check_freshness <val>         : check_freshness (Default: " << CHPCCNagiosToolSet::m_nCheckFreshness << ")\n";
    std::cout << "  -override_event_handler_enabled <val>   : event_handler_enabled (Default: " << CHPCCNagiosToolSet::m_nEventHandlerEnabled << ")\n";
    std::cout << "  -override_is_volatile <val>             : is_volatile (Default: " << CHPCCNagiosToolSet::m_nIsVolatile << ")\n";
    std::cout << "  -override_normal_check_interval <val>   : normal_check_interval (Default: " << CHPCCNagiosToolSet::m_nNormalCheckInterval << ")\n";
    std::cout << "  -override_retry_check_interval <val>    : retry_check_interval (Default: " << CHPCCNagiosToolSet::m_nRetryCheckInterval << ")\n";
    std::cout << "  -override_flap_detection_enabled <val>  : flap_detection_enabled (Default: " << CHPCCNagiosToolSet::m_nFlapDetectionEnabled << ")\n";
    std::cout << "  -override_process_perf_data <val>       : process_perf_data (Default: " << CHPCCNagiosToolSet::m_nProcessPerfData << ")\n";
    std::cout << "  -override_failure_prediction_enabled <val>  : failure_prediction_enabled (Default: " << CHPCCNagiosToolSet::m_nFailurePredictionEnabled << ")\n";
    std::cout << "  -override_retain_status_information <val>   : retain_status_information (Default: " << CHPCCNagiosToolSet::m_nRetainStatusInformation << ")\n";
    std::cout << "  -override_retain_nonstatus_information <val>: retain_nonstatus_information (Default: " << CHPCCNagiosToolSet::m_nRetainNonStatusInformation << ")\n";
    std::cout << "  -disable_check_all_disks        : disable disk space checks\n";
    std::cout << "  -disable_check_users            : disable user logged on checks\n";
    std::cout << "  -disable_check_procs            : disable process checks\n";
    std::cout << "  -disable_check_load             : disable load check\n";
    std::cout << "  -disable_use_of_note_for_host_port  : the send command will use the detail/note for host:ip instead of param (Default: true) \n";
    std::cout << "  -use_https                      : use https connection for esp service calls (HIGHLY RECOMMENDED when using username/password)\n";
    std::cout << "  -d or -debug                    : verbose debug output\n\n";
}

int main(int argc, char *argv[])
{
    InitModuleObjects();

    int i = 1;
    StringBuffer strConfigGenPath;
    StringBuffer strOutputPath;
    StringBuffer strEnvFilePath;
    StringBuffer strURL;
    StringArray strEclWatchHostPortArray;
    bool bGenerateHostGroup                 = false;
    bool bGenerateServiceAndHost            = false;
    bool bGenerateNRPECommands              = false;
    bool bGenerateEscalationCommands        = false;
    bool bUseDetailForHostPort              = true;

    if (argc == 1)
    {
        usage();
        return 0;
    }

    for (int c = 1; c < argc; c++)
    {
        CHPCCNagiosToolSet::m_strCommandLine.append(argv[c]).append(" ");
    }

    while (i < argc)
    {
        if (argc == 1 || stricmp(argv[i],"-h") == 0 || stricmp(argv[i],"-help") == 0)
        {
            usage();
            return 0;
        }
        else if (stricmp(argv[i], "-c") == 0 || stricmp(argv[i], "-cfggen") == 0)
        {
            i++;
            strConfigGenPath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-g") == 0 || stricmp(argv[i], "-hostgroup") == 0)
        {
            bGenerateHostGroup = true;
        }
        else if (stricmp(argv[i], "-s") == 0 || stricmp(argv[i], "-service") == 0)
        {
            bGenerateServiceAndHost = true;
        }
        else if (stricmp(argv[i], "-n") == 0 || stricmp(argv[i], "-nrpe") == 0)
        {
            CHPCCNagiosToolSet::m_bUseNPRE = true;
        }
        else if (stricmp(argv[i], "-o") == 0 || stricmp(argv[i], "-output") == 0 || stricmp(argv[i], "-out") == 0)
        {
            i++;
            strOutputPath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-e") == 0 || stricmp(argv[i], "-env") == 0)
        {
            i++;
            strEnvFilePath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-r") == 0 || stricmp(argv[i], "-retry") == 0)
        {
            CHPCCNagiosToolSet::m_retryHostNameLookUp = true;
        }
        else if (stricmp(argv[i], "-m") == 0 || stricmp(argv[i], "-commands") == 0)
        {
            bGenerateNRPECommands = true;
        }
        else if (stricmp(argv[i], "-ec") == 0 || stricmp(argv[i], "-escalation_cmds") == 0)
        {
            bGenerateEscalationCommands = true;
        }
        else if (stricmp(argv[i], "-enable_host_notify") == 0)
        {
            CHPCCNagiosToolSet::m_bEnableHostEscalations = true;
        }
        else if (stricmp(argv[i], "-enable_service_notify") == 0)
        {
            CHPCCNagiosToolSet::m_bEnableServiceEscalations = true;
        }
        else if (stricmp(argv[i], "-override_send_host_status") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pSendServiceStatus, argv[i], sizeof(CHPCCNagiosToolSet::m_pSendServiceStatus));
            }
            else
            {
                std::cout << "invalid override_send_host_status value\n";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_send_service_status") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pSendHostStatus, argv[i], sizeof(CHPCCNagiosToolSet::m_pSendHostStatus));
            }
            else
            {
                std::cout << "invalid override_send_service_statusl value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_service_status") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pServiceNotificatonCommand, argv[i], sizeof(CHPCCNagiosToolSet::m_pServiceNotificatonCommand));
            }
            else
            {
                std::cout << "invalid override_service_status value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_host_status") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pHostNotificatonCommand, argv[i], sizeof(CHPCCNagiosToolSet::m_pHostNotificatonCommand));
            }
            else
            {
                std::cout << "invalid override_host_status value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_eclwatch_host_port") == 0)
        {
            i++;
            strEclWatchHostPortArray.append(argv[i]);
        }
        else if (stricmp(argv[i], "-u") == 0 || stricmp(argv[i], "-user") == 0)
        {
            CHPCCNagiosToolSet::m_bUseAuthentication  = true;

            i++;
            strncpy(CHPCCNagiosToolSet::m_pUserMacro, argv[i], sizeof(CHPCCNagiosToolSet::m_pUserMacro));
        }
        else if (stricmp(argv[i], "-p") == 0 || stricmp(argv[i], "-pass") == 0)
        {
            CHPCCNagiosToolSet::m_bUseAuthentication = true;

            i++;
            strncpy(CHPCCNagiosToolSet::m_pPasswordMacro, argv[i], sizeof(CHPCCNagiosToolSet::m_pPasswordMacro));
        }
        else if (stricmp(argv[i], "-set_esp_username_pw") == 0)
        {
            CHPCCNagiosToolSet::m_bUseAuthentication  = true;
            static int nOverrideCount = 0;

            i++;
            (CHPCCNagiosToolSet::m_EspUserNamePWOverrides[nOverrideCount]).set(argv[i]);
            i++;
            (CHPCCNagiosToolSet::m_pUserMacroArray[nOverrideCount]).set(argv[i]);
            i++;
            (CHPCCNagiosToolSet::m_pPasswordMacroArray[nOverrideCount]).set(argv[i]);

            nOverrideCount++;
        }
        else if (stricmp(argv[i], "-d") == 0 || stricmp(argv[i], "-debug") == 0)
        {
            CHPCCNagiosToolSet::m_bVerbose = true;
            std::cout << "\n**verbose debug on**\n";
        }
        else if (stricmp(argv[i], "-attempts") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "\nInvalid attempts (-a) specified\n";
                exit(1);
            }
            CHPCCNagiosToolSet::m_uMaxCheckAttempts = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-disable_check_all_disks") == 0)
        {
            CHPCCNagiosToolSet::m_bCheckAllDisks  = false;
        }
        else if (stricmp(argv[i], "-disable_check_users") == 0)
        {
            CHPCCNagiosToolSet::m_bCheckUsers = false;
        }
        else if (stricmp(argv[i], "-disable_check_procs") == 0)
        {
            CHPCCNagiosToolSet::m_bCheckProcs = false;
        }
        else if (stricmp(argv[i], "-disable_check_load") == 0)
        {
            CHPCCNagiosToolSet::m_bCheckLoad = false;
        }

        else if (stricmp(argv[i], "-sysload1warn") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_fSystemLoad1Warn = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid sysload1warn value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-sysload5warn") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_fSystemLoad5Warn = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid sysload5warn value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-sysload15warn") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_fSystemLoad15Warn = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid sysload15warn value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-sysload1crit") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_fSystemLoad15Critical = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid sysload1crit value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-sysload5crit") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_fSystemLoad15Critical = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid sysload5crit value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-sysload15crit") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_fSystemLoad15Critical = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid sysload15crit value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-diskspacewarn") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nDiskSpacePercentageWarning = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid diskspacewarn value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-diskspacecrit") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nDiskSpacePercentageCritical = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid -diskspacecrit value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-usernumwarn") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nUserNumberWarning = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid usernumwarn value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-usernumcrit") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nUserNumberCritical = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid usernumcrit value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-totalprocswarn") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nTotalProcsWarning = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid totalprocswarn value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-totalprocscrit") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nTotalProcsCritical = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid totalprocscrit value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-checkperiod") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pCheckPeriod, argv[i], sizeof(CHPCCNagiosToolSet::m_pCheckPeriod));
            }
            else
            {
                std::cout << "invalid checkperiod value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-contacts") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pContacts, argv[i], sizeof(CHPCCNagiosToolSet::m_pContacts));
            }
            else
            {
                std::cout << "invalid contacts value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-contactgroups") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pContactGroups, argv[i], sizeof(CHPCCNagiosToolSet::m_pContactGroups));
            }
            else
            {
                std::cout << "invalid contactgroups value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-notify_interval") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                CHPCCNagiosToolSet::m_nNotificationInterval = atoi(argv[i]);
            }
            else
            {
                std::cout << "invalid notify_interval value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-notify_period") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pNotificationPeriod, argv[i], sizeof(CHPCCNagiosToolSet::m_pNotificationPeriod));
            }
            else
            {
                std::cout << "invalid notify_period value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_check_all_disks") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pCheckDiskSpace, argv[i], sizeof(CHPCCNagiosToolSet::m_pCheckDiskSpace));
            }
            else
            {
                std::cout << "invalid override_check_all_disks value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_check_users") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pCheckUsers, argv[i], sizeof(CHPCCNagiosToolSet::m_pCheckUsers));
            }
            else
            {
                std::cout << "invalid override_check_users value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_check_procs") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pCheckProcs, argv[i], sizeof(CHPCCNagiosToolSet::m_pCheckProcs));
            }
            else
            {
                std::cout << "invalid override_check_procs value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_retry_interval") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_retry_interval value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nRetryInteval = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_active_checks_enabled") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_active_checks_enabled value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nActiveChecksEnabled = atoi(argv[i]);

        }
        else if (stricmp(argv[i], "-override_check_load") == 0)
        {
            i++;

            if (argv[i] != NULL && *argv[i] != 0)
            {
                strncpy(CHPCCNagiosToolSet::m_pCheckLoad, argv[i], sizeof(CHPCCNagiosToolSet::m_pCheckLoad));
            }
            else
            {
                std::cout << "invalid override_check_load value";
                exit(1);
            }
        }
        else if (stricmp(argv[i], "-override_passive_checks_enabled") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_passive_checks_enabled value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nPassiveChecksEnabled = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_parallelize_check") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_parallelize_check value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nParallelizeCheck = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_obsess_over_service") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_obsess_over_service value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nObsessOverService = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_check_freshness") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_check_freshness value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nCheckFreshness = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_event_handler_enabled") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_event_handler_enabled value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nEventHandlerEnabled = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_is_volatile") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_is_volatile value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nIsVolatile = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_normal_check_interval") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_normal_check_interval value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nNormalCheckInterval = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_retry_check_interval") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_retry_check_interval value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nRetryCheckInterval = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_flap_detection_enabled") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_flap_detection_enabled value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nFlapDetectionEnabled = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_process_perf_data") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_process_perf_data value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nProcessPerfData = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_failure_prediction_enabled") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_failure_prediction_enabled value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nFailurePredictionEnabled = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_retain_status_information") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_retain_status_information value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nRetainStatusInformation = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-override_retain_nonstatus_information") == 0)
        {
            i++;

            if (argv[i] == NULL || *argv[i] == 0)
            {
                std::cout << "invalid override_retain_nonstatus_information value";
                exit(1);
            }
            CHPCCNagiosToolSet::m_nRetainNonStatusInformation = atoi(argv[i]);
        }
        else if (stricmp(argv[i], "-use_https") == 0)
        {
            CHPCCNagiosToolSet::m_bUseHTTPS = true;
        }
        else if (stricmp(argv[i], "-disable_use_of_note_for_host_port") == 0)
        {
            bUseDetailForHostPort = false;
        }
        else if (stricmp(argv[i], "-set_url") == 0)
        {
            i++;
            strURL.set(argv[i]);
        }
        i++;
    }

    try
    {
        if (strOutputPath.length() == 0)
        {
            std::cout << "Missing output file path! (-output)\n";
            return 0;
        }
        else if ( bGenerateServiceAndHost + bGenerateHostGroup + bGenerateNRPECommands + bGenerateEscalationCommands!= 1) //(bGenerateServiceAndHost^bGenerateHostGroup)^bGenerateNRPECommands) == false)
        {
            std::cout << "Select one (1) type of config per invocation (e.g. -hostgroup xor -service xor -nrpe xor -escalation_cmds)\n";
            return 0;
        }
        else if (bGenerateHostGroup == true)
        {
            std::cout << "Generating hostgroup --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
            {
                std::cout << "\nError generating configuration!. Verify input.\n";
                return 0;
            }
        }
        else if (bGenerateServiceAndHost == true)
        {
            std::cout << "Generating service and host config --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
            {
                std::cout << "\nError generating service and host configuration!. Verify input.\n";
                return 0;
            }
        }
        else if (bGenerateNRPECommands == true)
        {
            std::cout << "Generating nrpe client command config --> " << strOutputPath.str();
            std::flush(std::cout);
        }
        else if (bGenerateServiceAndHost^bGenerateHostGroup^bGenerateEscalationCommands == false)
        {
            std::cout << "Can only generate 1 type of config per invocation! (-hostgroup xor -service xor -escalation_cmds)\n";
            return 0;
        }
        else if (bGenerateServiceAndHost == false && bGenerateHostGroup == false && bGenerateEscalationCommands == false)
        {
            std::cout << "Nothing to generate! (-hostgroup xor -service)\n";
            return 0;
        }
        else if (bGenerateHostGroup == true)
        {
            std::cout << "Generating hostgroup --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
            {
                std::cout << "\nError generating configuration! Verify input.\n";
                return 0;
            }
        }
        else if (bGenerateServiceAndHost == true)
        {
            std::cout << "Generating service and host config --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
            {
                std::cout << "\nError generating service and host configuration! Verify input.\n";
                return 0;
            }
        }
        else if (bGenerateEscalationCommands == true)
        {
            std::cout << "Generating escalation command config --> " << strOutputPath.str();
            std::flush(std::cout);

            if (CHPCCNagiosToolSet::generateEscalationCommandConfigurationFile(strOutputPath.str(),
                                                                               strEclWatchHostPortArray,
                                                                               (CHPCCNagiosToolSet::m_bUseAuthentication == true) ? CHPCCNagiosToolSet::m_pUserMacro : NULL,
                                                                               (CHPCCNagiosToolSet::m_bUseAuthentication == true) ? CHPCCNagiosToolSet::m_pPasswordMacro : NULL,
                                                                               CHPCCNagiosToolSet::m_bUseHTTPS,
                                                                               bUseDetailForHostPort,
                                                                               strURL.str(),
                                                                               (strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()),
                                                                               (strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
                                                                                ) == false)
            {
                std::cout << "\nError generating command configuration! Verify input.\n";
                return 0;
            }
        }

        std::cout << "\nDone!\n";
    }
    catch (...)
    {
        std::cout << "\nException caught!\n  -->Check that files and directories exists and that permissions are set properly.\n";
    }

    return 0;
}
