#ifndef _HPCC_NAGIOSTOOLSET_HPP_
#define _HPCC_NAGIOSTOOLSET_HPP_

#include "HPCCNagiosToolSetCommon.hpp"

static const char *PCONFIGGEN_PATH("/opt/HPCCSystems/sbin/configgen");
static const char *PENV_XML("/etc/HPCCSystems/environment.xml");

class StringArray;
class StringBuffer;

class CHPCCNagiosToolSet
{
public:
    static bool generateHostGroupsConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServicesConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServerAndHostConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServicePluginConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);


protected:

    static bool generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosHostConfig(StringBuffer &strHostConfig, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool getConfiggenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, MemoryBuffer &memBuff);
    static void getHostName(StringBuffer &strHostName, const char *pIP,  MapIPtoNode &mapIPtoHostName);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
