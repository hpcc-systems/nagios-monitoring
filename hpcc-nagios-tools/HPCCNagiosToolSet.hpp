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
    static bool generateHostGroupFile(const char* pOutputFilePath, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateHostGroupFile(StringBuffer &strOutput, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServiceDefinitionFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServiceDefinitionFile(StringBuffer &strOutput, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

protected:

    //void createServiceDefinitionString(const char)
    void createHostGroupString(StringArray &pIP, StringBuffer &strHostGroup);
    static bool generateNagiosHostConfig(StringBuffer &strHostConfig, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
