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
    static bool generateHostGroupsFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateHostGroupsFile(StringBuffer &strOutput, const char* pEnvXML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServiceDefinitionFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServiceDefinitionFile(StringBuffer &strOutput, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

protected:

    //void createServiceDefinitionString(const char)
    static bool generateHostGroupConfig(StringArray &pIP, StringBuffer &strHostGroup);
    static bool generateNagiosHostConfig(StringBuffer &strHostConfig, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
