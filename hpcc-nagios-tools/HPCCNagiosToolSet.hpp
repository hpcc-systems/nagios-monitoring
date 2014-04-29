#ifndef _HPCC_NAGIOSTOOLSET_HPP_
#define _HPCC_NAGIOSTOOLSET_HPP_

#include "HPCCNagiosToolSetCommon.hpp"

static const char *PCONFIGGEN_PATH("/opt/HPCCSystems/sbin/configgen");
static const char *PENV_XML("/etc/HPCCSystems/environment.xml");

static const bool bDefaultCheckProcs    = true;
static const bool bDefaultCheckDisk     = true;
static const bool bDefaultCheckLoad     = true;
static const bool bDefaultCheckUsers    = true;

class StringArray;
class StringBuffer;

class CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosHostEvent(StringBuffer *pStrBuffer) : m_pStrBuffer(pStrBuffer)
    {
    }
    virtual ~CHPCCNagiosHostEvent()
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char* pToken) = 0;
protected:
    StringBuffer *m_pStrBuffer;
    CHPCCNagiosHostEvent()
    {
    }
};

class CHPCCNagiosToolSet
{
public:
    static bool generateHostGroupsConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServicesConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServerAndHostConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServicePluginConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

protected:

    static bool discoverHPCCHosts(CHPCCNagiosHostEvent &evHostEvent, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSSHCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosDaliCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSashaCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosRoxieCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosDafileSrvCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSystemCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH,\
                                                    bool bGenCheckProcs = bDefaultCheckProcs, bool bGenCheckDisk = bDefaultCheckDisk,\
                                                    bool bGenCheckUsers = bDefaultCheckUsers, bool bCheckLoad = bDefaultCheckLoad);
    static bool generateNagiosHostConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool getConfiggenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, MemoryBuffer &memBuff);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
