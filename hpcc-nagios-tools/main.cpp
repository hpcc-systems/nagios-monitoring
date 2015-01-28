#include "HPCCNagiosToolSet.hpp"
#include "jstring.hpp"
#include <iostream>

void usage()
{
    std::cout << "HPCC Systems Nagios configuration file generator (version: " << BUILD_VERSION_MAJOR  << "." << BUILD_VERSION_MINOR << "." << BUILD_VERSION_POINT << ")\n\
Usage: hpcc-nagios-tools -env <environment file> -out <output path> [options]\n\
Usage: hpcc-nagios-tools -env /tmp/env190.xml -u \"\\$USER3\\$\" -p \"\\$USER4\\$\" -o /etc/nagios3/conf.d/hpcc1services.cfg -s -usernumwarn 10  -usernumcrit 15 -disable_check_all_disks\n\n";
    std::cout << "  -c or -cfggen <val>     : The path to the configgen.  (Default: /opt/HPCCSystems/sbin/configgen)\n";
    std::cout << "  -g or -hostgroup        : generate host group file\n";
    std::cout << "  -s or -service          : generate service and host file\n";
    std::cout << "  -n or -nrpe             : generate client plugin cfgs for nrpe\n";
    std::cout << "  -e or -env <val>        : hpcc environment configuration file (Default: /etc/HPCCSystems/environment.xml)\n";
    std::cout << "  -o or -output <val>     : outpfile where the generated configuration will be written\n";
    std::cout << "  -r or -retry            : keep attempting to resolve IP to hostnames (Default: will stop resolution after 1st failure)\n";
    std::cout << "  -u or -user <val>       : macro to use for username for esp server login\n";
    std::cout << "  -p or -pass <val>       : macro to use for password for esp server login\n";
    std::cout << "  -attempts <val>         : max host retry attempts   (defaut: " <<  CHPCCNagiosToolSet::m_uMaxCheckAttempts << ")\n";
    std::cout << "  -sysload1warn <val>     : load1 warning threshold   (Default: " << CHPCCNagiosToolSet::m_fSystemLoad1Warn << ")\n";
    std::cout << "  -sysload5warn <val>     : load5 warning threshold   (Default: " << CHPCCNagiosToolSet::m_fSystemLoad5Warn << ")\n";
    std::cout << "  -sysload15warn <val>    : load15 warning threshold  (Default: " << CHPCCNagiosToolSet::m_fSystemLoad15Warn  << ")\n";
    std::cout << "  -sysload1crit <val>     : load1 critical threshold  (Default: " << CHPCCNagiosToolSet::m_fSystemLoad1Critical << ")\n";
    std::cout << "  -sysload5crit <val>     : load5 critical threshold  (Default: " << CHPCCNagiosToolSet::m_fSystemLoad5Critical << ")\n";
    std::cout << "  -sysload15crit <val>    : load15 critical threshold (Default: " << CHPCCNagiosToolSet::m_fSystemLoad15Critical << ")\n";
    std::cout << "  -diskspacewarn <val>    : disk space % warning threshold  (Default: " << CHPCCNagiosToolSet::m_nDiskSpacePercentageWarning  << ")\n";
    std::cout << "  -diskspacecrit <val>    : disk space % critical threshold (Default: " << CHPCCNagiosToolSet::m_nDiskSpacePercentageCritical << ")\n";
    std::cout << "  -usernumwarn <val>      : users logged in warning threshold  (Default: " << CHPCCNagiosToolSet::m_nUserNumberWarning << ")\n";
    std::cout << "  -usernumcrit <val>      : users logged in critical threshold (Default: " << CHPCCNagiosToolSet::m_nUserNumberCritical << ")\n";
    std::cout << "  -totalprocswarn <val>   : total process warning threshold   (Default: " << CHPCCNagiosToolSet::m_nTotalProcsWarning << ")\n";
    std::cout << "  -totalprocscrit <val>   : total process critical threshold  (Default: " << CHPCCNagiosToolSet::m_nTotalProcsCritical << ")\n";
    std::cout << "  -checkperiod <val>      : host check period (Default: " << CHPCCNagiosToolSet::m_pCheckPeriod << ")\n";
    std::cout << "  -contacts <val>         : host contacts     (Default: " << CHPCCNagiosToolSet::m_pContacts << ")\n";
    std::cout << "  -contactgroups <val>    : host contact groups (Default: " << CHPCCNagiosToolSet::m_pContactGroups << ")\n";
    std::cout << "  -notify_interval <val>  : host contact groups (Default: " << CHPCCNagiosToolSet::m_nNotificationInterval << ")\n";
    std::cout << "  -notify_period <val>    : host contact groups (Default: " << CHPCCNagiosToolSet::m_pNotificationPeriod << ")\n";
    std::cout << "  -override_check_all_disks <val> : check_all_disk plugin name (Default: " << CHPCCNagiosToolSet::m_pCheckDiskSpace << ")\n";
    std::cout << "  -override_check_users <val>     : check_users plugin name (Default: " << CHPCCNagiosToolSet::m_pCheckUsers << ")\n";
    std::cout << "  -override_check_procs <val>     : check_procs plugin name (Default: " << CHPCCNagiosToolSet::m_pCheckProcs << ")\n";
    std::cout << "  -override_check_load <val>      : check_load plugin name  (Default: " << CHPCCNagiosToolSet::m_pCheckLoad << ")\n";
    std::cout << "  -disable_check_all_disks        : disable disk space checks\n";
    std::cout << "  -disable_check_users            : disable user logged on checks\n";
    std::cout << "  -disable_check_procs            : disable process checks\n";
    std::cout << "  -disable_check_load             : disable load check\n";
    std::cout << "  -d or -debug                    : verbose debug output\n";
}

int main(int argc, char *argv[])
{
    InitModuleObjects();

    int i = 1;
    StringBuffer strConfigGenPath;
    StringBuffer strOutputPath;
    StringBuffer strEnvFilePath;
    bool bGenerateHostGroup         = false;
    bool bGenerateServiceAndHost    = false;
    bool bGenerateNRPECommands      = false;

    if (argc == 1)
    {
        usage();
        return 0;
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
        i++;
    }

    try
    {
        if (strOutputPath.length() == 0)
        {
            std::cout << "Missing output file path! (-output)\n";
            return 0;
        }
        else if ( bGenerateServiceAndHost + bGenerateHostGroup + bGenerateNRPECommands != 1) //(bGenerateServiceAndHost^bGenerateHostGroup)^bGenerateNRPECommands) == false)
        {
            std::cout << "Select one (1) type of config per invocation (e.g. -hostgroup xor -service xor -nrpe)\n";
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
        else if (bGenerateServiceAndHost^bGenerateHostGroup == false)
        {
            std::cout << "Can only generate 1 type of config per invocation! (-hostgroup xor -service)\n";
            return 0;
        }
        else if (bGenerateServiceAndHost == false && bGenerateHostGroup == false)
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

        std::cout << "\nDone!\n";
    }
    catch (...)
    {
        std::cout << "\nException caught!\n  -->Check that files and directories exists and that permissions are set properly.\n";
    }

    return 0;
}
