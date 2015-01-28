#ifndef _HPCC_NAGIOSTOOLSET_HPP_
#define _HPCC_NAGIOSTOOLSET_HPP_

#include "HPCCNagiosToolSetCommon.hpp"
#include "build-config.h"

static const char *PCONFIGGEN_PATH(ADMIN_DIR"/configgen");
static const char *PENV_XML(CONFIG_DIR"/environment.xml");

class StringArray;
class StringBuffer;

#define BUFFER_SIZE_1   64
#define BUFFER_SIZE_2   BUFFER_SIZE_1*4

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

    static bool m_bVerbose;
    static bool m_retryHostNameLookUp;
    static bool m_bUseNPRE;
    static bool m_bUseAuthentication;
    static bool m_bCheckAllDisks;
    static bool m_bCheckUsers;
    static bool m_bCheckLoad;
    static bool m_bCheckProcs;

    static char m_pNRPE[BUFFER_SIZE_1];
    static char m_pUserMacro[BUFFER_SIZE_1];
    static char m_pPasswordMacro[BUFFER_SIZE_1];

    static char m_pCheckPeriod[BUFFER_SIZE_1];
    static char m_pContacts[BUFFER_SIZE_2];
    static char m_pContactGroups[BUFFER_SIZE_2];
    static int  m_nNotificationInterval;
    static char m_pNotificationPeriod[BUFFER_SIZE_2];

    static char m_pCheckProcs[BUFFER_SIZE_2];
    static char m_pCheckDiskSpace[BUFFER_SIZE_2];
    static char m_pCheckUsers[BUFFER_SIZE_2];
    static char m_pCheckLoad[BUFFER_SIZE_2];

    static int m_uMaxCheckAttempts;
    static int m_nDiskSpacePercentageWarning;
    static int m_nDiskSpacePercentageCritical;
    static int m_nUserNumberWarning;
    static int m_nUserNumberCritical;
    static int m_nTotalProcsWarning;
    static int m_nTotalProcsCritical;

    static float m_fSystemLoad1Warn;
    static float m_fSystemLoad5Warn;
    static float m_fSystemLoad15Warn;
    static float m_fSystemLoad1Critical;
    static float m_fSystemLoad5Critical;
    static float m_fSystemLoad15Critical;

    static bool generateHostGroupsConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateServerAndHostConfigurationFile(const char* pOutputFilePath, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

protected:

    static bool generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosDaliCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSashaCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosRoxieCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosDafileSrvCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSystemCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosHostConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosNRPEClientConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

private:

    static char* invokeConfigGen(const char* pEnvXML, const char* pConfigGenPath, const char *pCmd = P_CONFIGGEN_PARAM_LIST_ALL, const char *pType = NULL);
    static bool getConfigGenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, StringBuffer &strBuff);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
