#include "HPCCNagiosToolSet.hpp"
#include "jstring.hpp"
#include <iostream>
#include "jstring.hpp"

void usage()
{
    std::cout << "HPCC Systems Nagios configuration file generator\n\
Usage: hpcc-nagios-tools -env <environment file> -out <output path> [options]\n";
    std::cout << "  -c or -cfggen     : The path to the configgen.  (Default: /opt/HPCCSystems/sbin/configgen)\n";
    std::cout << "  -g or -hostgroup  : generate host group file\n";
    std::cout << "  -s or -service    : generate service and host file\n";
    std::cout << "  -p or -plugin     : generate configuration for plugins\n";
    std::cout << "  -e or -env        : hpcc environment configuration file (Default: /etc/HPCCSystems/environment.xml)\n";
    std::cout << "  -o or -output     : outpfile where the generated configuration will be written\n";

}

int main(int argc, char *argv[])
{
    int i = 1;
    StringBuffer strConfigGenPath;
    StringBuffer strOutputPath;
    StringBuffer strEnvFilePath;
    bool bGenerateHostGroup = false;
    bool bGenerateServiceAndHost   = false;
    bool bGeneratePluginConfig = false;

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
        else if (stricmp(argv[i], "-o") == 0 || stricmp(argv[i], "-output") == 0)
        {
            i++;
            strOutputPath.set(argv[i]);
        }
        else if (stricmp(argv[i], "-e") == 0 || stricmp(argv[i], "-env") == 0)
        {
            i++;
            strEnvFilePath.set(argv[i]);
        }
        else if (stricmp(argv[i], "p") == 0 || stricmp(argv[i], "-plugin") == 0)
        {
            bGeneratePluginConfig =  true;
        }

        i++;
    }

    if (strOutputPath.length() == 0)
    {
        std::cout << "Missing output file path! (-output)\n";
        return 0;
    }
    else if (bGenerateServiceAndHost^bGenerateHostGroup^bGeneratePluginConfig == false)
    {
        std::cout << "Can only generate 1 type of config per invocation! (-hostgroup xor -service xor -plugin)\n";
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
        if (CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
        {
            std::cout << "\nError generating configuration!. Verify input.\n";
            return 0;
        }
    }
    else if (bGeneratePluginConfig == true)
    {
        std::cout << "Generating plugin config --> " << strOutputPath.str();

        if (CHPCCNagiosToolSet::generateServicePluginConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
        {
            std::cout << "\nError generating plugin configuration!. Verify input.\n";
            return 0;
        }
    }
    else if (bGenerateServiceAndHost == true)
    {
        std::cout << "Generating service and host config --> " << strOutputPath.str();

        if (CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(strOutputPath.str(), strEnvFilePath.length() == 0 ? NULL : strEnvFilePath.str()) == false, strConfigGenPath.length() == 0 ? NULL : strConfigGenPath.str())
        {
            std::cout << "\nError generating service and host configuration!. Verify input.\n";
            return 0;
        }
    }

    std::cout << "\nDone!\n";
    return 0;
}
