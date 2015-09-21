#ifndef _HPCC_NAGIOSTOOLSET_HPP_
#define _HPCC_NAGIOSTOOLSET_HPP_

#include "HPCCNagiosToolSetCommon.hpp"
#include "build-config.h"
#include "jutil.hpp"

static const char *PCONFIGGEN_PATH(ADMIN_DIR"/configgen");
static const char *PENV_XML(CONFIG_DIR"/environment.xml");

class StringArray;
class StringBuffer;

#define BUFFER_SIZE_1   64
#define BUFFER_SIZE_2   BUFFER_SIZE_1*4
#define URL_BUFFER_SIZE 2048
#define MAX_CUSTOM_VARS 32

class CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosHostEvent(StringBuffer *pStrBuffer) : m_pStrBuffer(pStrBuffer)
    {
    }
    virtual ~CHPCCNagiosHostEvent()
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char* pToken, const char* pNote = NULL) = 0;
protected:
    StringBuffer *m_pStrBuffer;
    CHPCCNagiosHostEvent()
    {
    }
};

class CHPCCNagiosToolSet
{
public:

    friend class CHPCCNagiosHostEventForSSH;
    friend class CHPCCNagiosHostEventHostConfig;
    friend class CHPCCNagiosNRPEClientEventConfig;

    static bool m_bVerbose;
    static bool m_retryHostNameLookUp;
    static bool m_bUseNPRE;
    static bool m_bUseAuthentication;
    static bool m_bCheckAllDisks;
    static bool m_bCheckUsers;
    static bool m_bCheckLoad;
    static bool m_bCheckProcs;
    static bool m_bEnableServiceEscalations;
    static bool m_bEnableHostEscalations;
    static bool m_bUseHTTPS;

    static char m_pNRPE[BUFFER_SIZE_1];
    static char m_pSeparator[BUFFER_SIZE_1];
    static char m_pUserMacro[BUFFER_SIZE_1];
    static char m_pPasswordMacro[BUFFER_SIZE_1];
    static StringBuffer m_pUserMacroArray[MAX_CUSTOM_VARS];  // support for 32 user variables
    static StringBuffer m_pPasswordMacroArray[MAX_CUSTOM_VARS];
    static StringBuffer m_EspUserNamePWOverrides[MAX_CUSTOM_VARS];
    static StringBuffer m_strCommandLine;

    static int  m_pCheckInterval;
    static char m_pCheckPeriod[BUFFER_SIZE_1];
    static char m_pContacts[BUFFER_SIZE_2];
    static char m_pContactGroups[BUFFER_SIZE_2];
    static int  m_nNotificationInterval;
    static char m_pNotificationPeriod[BUFFER_SIZE_2];
    static int  m_nRetryInteval;
    static int  m_nActiveChecksEnabled;
    static int  m_nPassiveChecksEnabled;
    static int  m_nParallelizeCheck;
    static int  m_nObsessOverService;
    static int  m_nCheckFreshness;
    static int  m_nEventHandlerEnabled;
    static int  m_nFlapDetectionEnabled;
    static int  m_nFailurePredictionEnabled;
    static int  m_nProcessPerfData;
    static int  m_nRetainStatusInformation;
    static int  m_nRetainNonStatusInformation;
    static int  m_nIsVolatile;
    static int  m_nNormalCheckInterval;
    static int  m_nRetryCheckInterval;
    static int  m_nEnabled;
    static int  m_nDisabled;

    static char m_pCheckProcs[BUFFER_SIZE_2];
    static char m_pCheckDiskSpace[BUFFER_SIZE_2];
    static char m_pCheckUsers[BUFFER_SIZE_2];
    static char m_pCheckLoad[BUFFER_SIZE_2];
    static char m_pSendServiceStatus[BUFFER_SIZE_2];
    static char m_pSendHostStatus[BUFFER_SIZE_2];
    static char m_pDevNULL[BUFFER_SIZE_1];
    static char m_pServiceNotificatonCommand[BUFFER_SIZE_2];
    static char m_pHostNotificatonCommand[BUFFER_SIZE_2];
    static char m_pNotificationURL[URL_BUFFER_SIZE];

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
    static bool generateEscalationCommandConfigurationFile(const char* pOutputFilePath, const StringArray &strEclWatchHostPortArray, const char* pUserMacro =  NULL,
                                                           const char* pPasswordMacro = NULL, bool bUseHTTPS = false, bool bAppendPortFromDetail = false,
                                                           const char *pURL = NULL, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

protected:

    static bool generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosDaliCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosThorCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSashaCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosRoxieCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosDafileSrvCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosSystemCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosHostConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosNRPEClientConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);
    static bool generateNagiosServiceEscalationConfig(StringBuffer &strServiceConfig, const char* pDisplayName, const char* pNote);
    static bool generateNagiosHostEscalationConfig(StringBuffer &strServiceConfig);
    static bool generateNagiosEscalationCommandConfig(StringBuffer &strCommandConfig, const StringArray &strEclWatchHostPortArray, const char *pUserMacro = NULL,
                                                      const char* pPasswordMacro = NULL, bool bUseHTTPS = false, bool bAppendHostPortFromDetail = false,
                                                      const char *pURL = NULL, const char* pEnvXML = PENV_XML, const char* pConfigGenPath = PCONFIGGEN_PATH);

private:

    static char* invokeConfigGen(const char* pEnvXML, const char* pConfigGenPath, const char *pCmd = P_CONFIGGEN_PARAM_LIST_ALL, const char *pType = NULL, const char *pCmdSuffix = "");
    static bool getConfigGenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, StringBuffer &strBuff);
    static bool addCommonParamsToSendStatus(StringBuffer &strCommandConfig, const char* pUserMacro, const char* pPasswordMacro, const bool bUseHTTPS, const bool bAppendHostPortFromDetail,
                                            const char  *pURL);
    static const char* getEspUserName(const char *pEspName, const char* pDefaultUserName);
    static const char* getEspPassword(const char *pEspName, const char* pDefaultPassword);
    static void appendHeader(StringBuffer &strBuffer);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
