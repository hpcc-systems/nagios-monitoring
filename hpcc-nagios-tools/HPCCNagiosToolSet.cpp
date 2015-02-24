#include <cstdio>
#include "jstream.ipp"
#include "jfile.hpp"
#include "jbuff.hpp"
#include "HPCCNagiosToolSet.hpp"
#include "HPCCNagiosToolSetCommon.hpp"
#include <cstring>
#include "XMLTags.h"
#include <iostream>

#define BUFFER_SIZE_3 1024

const char *P_DALI("dali");
const char *P_SASHA("sasha");
const char *P_ROXIE("roxie");
const char *P_DAFILESRV("dafilesrv");

static bool bDoLookUp = true;

bool CHPCCNagiosToolSet::m_bVerbose                              = false;
bool CHPCCNagiosToolSet::m_retryHostNameLookUp                 = false;
bool CHPCCNagiosToolSet::m_bUseNPRE                             = false;
bool CHPCCNagiosToolSet::m_bUseAuthentication                   = false;
bool CHPCCNagiosToolSet::m_bCheckAllDisks                       = true;
bool CHPCCNagiosToolSet::m_bCheckLoad                           = true;
bool CHPCCNagiosToolSet::m_bCheckUsers                          = true;
bool CHPCCNagiosToolSet::m_bCheckProcs                          = true;
char CHPCCNagiosToolSet::m_pNRPE[BUFFER_SIZE_1]                 = {""};
char CHPCCNagiosToolSet::m_pUserMacro[BUFFER_SIZE_1]            = {};
char CHPCCNagiosToolSet::m_pPasswordMacro[BUFFER_SIZE_1]        = {};
char CHPCCNagiosToolSet::m_pCheckPeriod[BUFFER_SIZE_1]          = {"24x7"};
char CHPCCNagiosToolSet::m_pContacts[BUFFER_SIZE_2]             = {"root"};
char CHPCCNagiosToolSet::m_pContactGroups[BUFFER_SIZE_2]        = {"admins"};
int  CHPCCNagiosToolSet::m_nNotificationInterval                = 10;
char CHPCCNagiosToolSet::m_pNotificationPeriod[BUFFER_SIZE_2]   = {"24x7"};

char CHPCCNagiosToolSet::m_pCheckProcs[BUFFER_SIZE_2]           = {"check_procs"};
char CHPCCNagiosToolSet::m_pCheckDiskSpace[BUFFER_SIZE_2]       = {"check_all_disks"};
char CHPCCNagiosToolSet::m_pCheckUsers[BUFFER_SIZE_2]           = {"check_users"};
char CHPCCNagiosToolSet::m_pCheckLoad[BUFFER_SIZE_2]            = {"check_load"};

int CHPCCNagiosToolSet::m_uMaxCheckAttempts             = 5;
int CHPCCNagiosToolSet::m_nDiskSpacePercentageWarning   = 15;
int CHPCCNagiosToolSet::m_nDiskSpacePercentageCritical  = 8;
int CHPCCNagiosToolSet::m_nUserNumberWarning            = 5;
int CHPCCNagiosToolSet::m_nUserNumberCritical           = 10;
int CHPCCNagiosToolSet::m_nTotalProcsWarning            = 350;
int CHPCCNagiosToolSet::m_nTotalProcsCritical           = 500;
float CHPCCNagiosToolSet::m_fSystemLoad1Warn            = 5.0;
float CHPCCNagiosToolSet::m_fSystemLoad5Warn            = 4.0;
float CHPCCNagiosToolSet::m_fSystemLoad15Warn           = 3.0;
float CHPCCNagiosToolSet::m_fSystemLoad1Critical        = 10.0;
float CHPCCNagiosToolSet::m_fSystemLoad5Critical        = 6.0;
float CHPCCNagiosToolSet::m_fSystemLoad15Critical       = 4.0;

class CHPCCNagiosHostEventForSSH : public CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosHostEventForSSH(StringBuffer *pStrBuffer) : CHPCCNagiosHostEvent(pStrBuffer)
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char *pToken)
    {
        if (pHostName == NULL || *pHostName == 0)
        {
            return;
        }
        else
        {
            m_pStrBuffer->append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).append("check for ssh connectivity")\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_SSH_SERVICE);

            if (CHPCCNagiosToolSet::m_bUseNPRE == false)
            {
                m_pStrBuffer->append(P_NAGIOS_SEPERATOR).append(CHPCCNagiosHostEventForSSH::m_nTimeOut);
            }

            m_pStrBuffer->append(P_NAGIOS_SERVICE_END_BRACKET);

            m_pStrBuffer->append("\n");
        }
    }
    static const int m_nTimeOut;

protected:

    CHPCCNagiosHostEventForSSH()
    {
    }
};

class CHPCCNagiosHostEventHostConfig : public CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosHostEventHostConfig(StringBuffer *pStrBuffer) : CHPCCNagiosHostEvent(pStrBuffer)
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char *pToken)
    {
        if (pHostName == NULL || *pHostName == 0)
        {
            return;
        }
        else
        {
            m_pStrBuffer->append(P_NAGIOS_HOST_CONFIG_1).append(pHostName).append(P_NAGIOS_HOST_CONFIG_ALIAS).append(pHostName).append(" ")\
                    .append(idx).append(P_NAGIOS_HOST_CONFIG_ADDRESS).append(pToken)\
                    .append(P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS).append(CHPCCNagiosToolSet::m_uMaxCheckAttempts)\
                    .append(P_NAGIOS_HOST_CONFIG_CHECK_PERIOD).append(CHPCCNagiosToolSet::m_pCheckPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_CONTACTS).append(CHPCCNagiosToolSet::m_pContacts)\
                    .append(P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS).append(CHPCCNagiosToolSet::m_pContactGroups)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_INTERVAL).append(CHPCCNagiosToolSet::m_nNotificationInterval)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD).append(CHPCCNagiosToolSet::m_pNotificationPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_END);
            m_pStrBuffer->append("\n");
        }
    }
protected:
    CHPCCNagiosHostEventHostConfig()
    {
    }
};

class CHPCCNagiosNRPEClientEventConfig : public CHPCCNagiosHostEvent
{
public:
    CHPCCNagiosNRPEClientEventConfig(StringBuffer *pStrBuffer) : CHPCCNagiosHostEvent(pStrBuffer)
    {
    }
    virtual void onHostEvent(const char *pHostName, int idx, const char *pToken)
    {
        if (pHostName == NULL || *pHostName == 0)
        {
            return;
        }
        else
        {
            m_pStrBuffer->append(P_NAGIOS_HOST_CONFIG_1).append(pHostName).append(P_NAGIOS_HOST_CONFIG_ALIAS).append(pHostName).append(" ")\
                    .append(idx).append(P_NAGIOS_HOST_CONFIG_ADDRESS).append(pToken)\
                    .append(P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS).append(CHPCCNagiosToolSet::m_uMaxCheckAttempts)\
                    .append(P_NAGIOS_HOST_CONFIG_CHECK_PERIOD).append(CHPCCNagiosToolSet::m_pCheckPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_CONTACTS).append(CHPCCNagiosToolSet::m_pContacts)\
                    .append(P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS).append(CHPCCNagiosToolSet::m_pContactGroups)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_INTERVAL).append(CHPCCNagiosToolSet::m_nNotificationInterval)\
                    .append(P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD).append(CHPCCNagiosToolSet::m_pNotificationPeriod)\
                    .append(P_NAGIOS_HOST_CONFIG_END);
            m_pStrBuffer->append("\n");
        }
    }
protected:
    CHPCCNagiosNRPEClientEventConfig()
    {
    }
};


const int CHPCCNagiosHostEventForSSH::m_nTimeOut = 10;

bool CHPCCNagiosToolSet::generateNagiosHostConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_MACHINES);

    if (pOutput == NULL)
    {
        return false;
    }

    int nCount = 0;

    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    int i = 0;
    while (pch != NULL)
    {
        if (nCount % 2 ==  0) // Process name
        {
            char pHostName[BUFFER_SIZE_3] = "";
            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }

            evHost.onHostEvent(pHostName, i, pch);

            struct NodeName nm;

            nm.strHostName.set(pHostName);
            nm.strHostAlias.setf("%s %d", pHostName, i);
            mapIPtoHostName.setValue(pch, nm);

            i++;
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath);

    if (m_bVerbose == true)
    {
        std::cout << "\nOutput:\n" << pOutput << "\n";
    }

    if (pOutput == NULL)
    {
        return false;
    }

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    int nCount = 0;
    char pProcess[BUFFER_SIZE_3] = "";
    bool bAdd = false;
    char *pch = NULL;
    char pHostName[BUFFER_SIZE_3] = "";
    StringBuffer strHostConfig;
    int i = 0;

    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % 6 ==  0) // Process name
        {
            if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_END_BRACKET);
                strHostConfig.append("\n");
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                bAdd = true;
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
                bAdd = false;
            }
        }
        else if (nCount % 6 == 2) // IP address
        {
            hostent *hp;

            memset(pHostName,0,sizeof(pHostName));

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                if (strcmp(pch, pHostName) == 0)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;

                    continue;
                }
                strcpy(pHostName, pch);
            }
            else
            {
                if (strcmp(pch, pHostName) == 0)
                {
                    pch = strtok(NULL, ",\n");
                    nCount++;

                    continue;
                }
                strcpy(pHostName,hp->h_name);
            }

            static char pLastHostName[BUFFER_SIZE_3] = "";

            if (bAdd == true)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_CONFIG_1).append(pProcess).append("-servers").append(P_NAGIOS_HOSTS_GROUP_ALIAS).append(pProcess).append(" servers")\
                                .append(P_NAGIOS_HOSTS_GROUP_MEMBERS).append(pHostName);

                strcpy(pLastHostName,pHostName);
                bAdd = false;
            }
            else
            {
                if (strcmp(pLastHostName,pHostName) != 0)
                {
                    strHostConfig.append(", ").append(pHostName);
                    strcpy(pLastHostName,pHostName);
                }
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    if (strHostConfig.length() > 0)
    {
        strHostConfig.append(P_NAGIOS_HOSTS_GROUP_END_BRACKET);
    }

    io->write(0, strHostConfig.length(), strHostConfig.str());
    io->close();

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosNRPEClientConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath);

    free(pOutput);
}

bool CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pOutputFilePath == NULL || *pOutputFilePath == 0)
    {
        return false;
    }

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath);

    if (pOutput == NULL)
    {
        return false;
    }

    const int nNumValues = 6;
    int nCount = 0;
    char pProcess[BUFFER_SIZE_3] = "";

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    StringBuffer strServiceConfig;
    MapIPtoNode mapIPtoHostName;
    CHPCCNagiosHostEventHostConfig evHost(&strServiceConfig);
    CHPCCNagiosToolSet::generateNagiosHostConfig(evHost, mapIPtoHostName, pEnvXML, pConfigGenPath);

    int i = -1;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && (strcmp(pch,XML_TAG_ESPPROCESS) == 0) || strcmp(pch, XML_TAG_DALISERVERPROCESS) == 0\
                || strcmp(pch, XML_TAG_SASHA_SERVER_PROCESS) == 0 || strcmp(pch, XML_TAG_DAFILESERVERPROCESS) == 0)
            {
                i++;
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    if (CHPCCNagiosToolSet::m_bUseNPRE == true)
    {
        strncpy(CHPCCNagiosToolSet::m_pNRPE, P_CHECK_NRPE_1_ARG, sizeof(CHPCCNagiosToolSet::m_pNRPE));
    }

    CHPCCNagiosToolSet::generateNagiosEspServiceConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosDaliCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosSashaCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosRoxieCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosDafileSrvCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosSystemCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);

    CHPCCNagiosHostEventForSSH event(&strServiceConfig);
    MapIPtoNode map;
    CHPCCNagiosToolSet::generateNagiosHostConfig(event,map, pEnvXML, pConfigGenPath);

    io->write(0, strServiceConfig.length(), strServiceConfig.str());
    io->close();

    free(pOutput);
    return true;
}

bool CHPCCNagiosToolSet::getConfigGenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, StringBuffer &strBuff)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    int nRetCode = 0;

    Owned<IPipeProcess> pipe = createPipeProcess();

    if (pipe->run("configgen", pCommandLine, ".", false, true, true, 0))
    {
        nRetCode = pipe->wait();

        if (nRetCode != 0)
        {
            std::cerr << "\nERROR " << nRetCode << ": unable to execute " << pCommandLine << "\n";
            exit(1);
        }

        Owned<ISimpleReadStream> pipeReader = pipe->getOutputStream();
        const size32_t chunkSize = 8192;
        int nTotalBytes = 0;

        loop
        {
            size32_t sizeRead = pipeReader->read(chunkSize, strBuff.reserve(chunkSize));

            nTotalBytes += sizeRead;

            if (sizeRead < chunkSize)
            {
                strBuff.setLength(strBuff.length() - (chunkSize - sizeRead));
                break;
            }
        }
        pipe->closeOutput();
    }

    if (CHPCCNagiosToolSet::m_bVerbose == true)
    {
        std::cout << "Buffer->\n" << strBuff.str() << "\n<--\n";
    }

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ESP_SERVICES);

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pServiceName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % 7 ==  0) // Process type
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ESPPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only EspProcess
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        if (nCount % 7 ==  1) // Process name
        {
            strncpy(pProcessName, pch, sizeof(pProcessName));
        }
        else if (nCount % 7 == 3) // service name
        {
            strncpy(pServiceName,pch, sizeof(pServiceName));
        }
        else if (nCount % 7 == 4) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % 7 == 5) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % 7 == 6) // protocol
        {
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION)\
                    .appendf("check for %s service for instance %s of type %s", pServiceName, pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(m_pNRPE).append(P_CHECK_ESP_SERVICE).append( strcmp(pch,P_HTTP) == 0 ? P_HTTP : P_HTTPS)\
                    .append(CHPCCNagiosToolSet::m_bUseAuthentication ? "_auth" : "");

            if (m_bUseNPRE == false)
            {
                strServiceConfig.appendf("%s%s", P_NAGIOS_SEPERATOR, strPort.str());
            }

            if (m_bUseAuthentication == true)
            {
		strServiceConfig.appendf("%s%s%s%s", P_NAGIOS_SEPERATOR, m_pUserMacro, P_NAGIOS_SEPERATOR, m_pPasswordMacro);
            }

	    strServiceConfig.appendf("%s\n", P_NAGIOS_SERVICE_END_BRACKET);
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosDaliCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_DALI);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_DALISERVERPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only Dali
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 3) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % nNumValues == 4)
        {
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_DALI);

            if (CHPCCNagiosToolSet::m_bUseNPRE == false)
            {
                strServiceConfig.append(P_NAGIOS_SEPERATOR).append(strPort.str()).append(P_NAGIOS_SEPERATOR).append(DALI_CHECK_TIMEOUT);
            }

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosSashaCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_SASHA);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_SASHA_SERVER_PROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only sasha
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 3) // IP Port
        {
            strPort.clear().append(pch);
        }
        else if (nCount % nNumValues == 4)
        {
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_SASHA);

            if (CHPCCNagiosToolSet::m_bUseNPRE == false)
            {
                strServiceConfig.append(P_NAGIOS_SEPERATOR).append(strPort.str()).append(P_NAGIOS_SEPERATOR).append(SASHA_CHECK_TIMEOUT);
            }

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosRoxieCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML , const char* pConfigGenPath)
{
    const int nNumValues = 5;
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_ROXIE);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ROXIE_SERVER) != 0)
            {
                free(pOutput);
                return false;  // expecting only roxie
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 4)
        {
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_ROXIE);

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosSystemCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 2;
    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_MACHINES);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues == 0) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
            if (CHPCCNagiosToolSet::m_bCheckProcs == true)
            {
                // processes
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckProcs)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckProcs);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_nTotalProcsWarning)\
                        .append(P_NAGIOS_SEPERATOR).append(m_nTotalProcsCritical);
                }
                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }

            if (CHPCCNagiosToolSet::m_bCheckAllDisks  == true)
            {
                // disk space
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckDiskSpace)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckDiskSpace);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_nDiskSpacePercentageWarning)\
                        .append(P_NAGIOS_SEPERATOR).append(m_nDiskSpacePercentageCritical);
                }
                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }

            if (CHPCCNagiosToolSet::m_bCheckUsers == true)
            {
                // number of users
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckUsers)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckUsers);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_nUserNumberWarning).append(P_NAGIOS_SEPERATOR).append(m_nUserNumberCritical);
                }

                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }

            if (CHPCCNagiosToolSet::m_bCheckLoad  == true)
            {
                // system load
                strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s", m_pCheckLoad)\
                        .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(m_pCheckLoad);

                if (CHPCCNagiosToolSet::m_bUseNPRE == false)
                {
                    strServiceConfig.append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad1Warn)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad5Warn)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad15Warn)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad1Critical)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad5Critical)\
                        .append(P_NAGIOS_SEPERATOR).append(m_fSystemLoad15Critical);
                }
                strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
            }
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosDafileSrvCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    char *pOutput = CHPCCNagiosToolSet::invokeConfigGen(pEnvXML, pConfigGenPath, P_CONFIGGEN_PARAM_LIST_ALL, P_DAFILESRV);

    if (pOutput == NULL)
    {
        return false;
    }

    int i = -1;
    char pProcess[BUFFER_SIZE_3] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strIPAddress;
    char pHostName[BUFFER_SIZE_3] = "";
    char pProcessName[BUFFER_SIZE_3] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_DAFILESERVERPROCESS) != 0)
            {
                free(pOutput);
                return false;  // expecting only dafilesrvprocess
            }
            else if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i = 0;
            }
            else if (pProcess == NULL || *pProcess == 0 || strcmp(pProcess, pch) != 0)
            {
                strncpy(pProcess, pch, sizeof(pProcess));
                i++;
            }
            else if (strcmp(pProcess,pch) == 0)
            {
                i++;
            }
        }
        else if (nCount % nNumValues == 1) // process name
        {
            strcpy(pProcessName,pch);
        }
        else if (nCount % nNumValues == 2) // IP Address
        {
            strIPAddress.clear().append(pch);

            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp =  m_retryHostNameLookUp;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }
        }
        else if (nCount % nNumValues == 4)
        {
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_DESCRIPTION).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CHECK_COMMAND).append(CHPCCNagiosToolSet::m_pNRPE).append(P_CHECK_DAFILESRV);

            strServiceConfig.append(P_NAGIOS_SERVICE_END_BRACKET).append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    free(pOutput);

    return true;
}

char* CHPCCNagiosToolSet::invokeConfigGen(const char* pEnvXML, const char* pConfigGenPath, const char *pCmd, const char *pType)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (pCmd == NULL || *pCmd == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return NULL;
    }

    StringBuffer strBuff;
    StringBuffer strConfigGenCmdLine(pConfigGenPath);

    strConfigGenCmdLine.append(pCmd).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    if (pType != NULL && *pType != 0)
    {
        strConfigGenCmdLine.append(P_BY_TYPE).append(pType);
    }

    CHPCCNagiosToolSet::getConfigGenOutput(pEnvXML, pConfigGenPath, strConfigGenCmdLine.str(), strBuff);

    if (m_bVerbose == true)
    {
        std::cout << "configen cmd line: " << strConfigGenCmdLine.str() << std::endl;
        std::cout << "original :\n " << strBuff.str() << std::endl;
    }

    strBuff.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters

    if (pType == NULL || *pType == 0)
    {
        strBuff.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    }

    char *pOutput = strdup(strBuff.str());

    if (m_bVerbose == true)
    {
        std::cout << "returning: \n" << pOutput << std::endl;
    }

    return pOutput;
}
