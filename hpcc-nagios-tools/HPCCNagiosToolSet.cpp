#include <cstdio>
#include "jstream.ipp"
#include "jfile.hpp"
#include "jbuff.hpp"
#include "HPCCNagiosToolSet.hpp"
#include "HPCCNagiosToolSetCommon.hpp"
#include <cstring>
#include "XMLTags.h"

const char *P_DALI("dali");
const char *P_SASHA("sasha");
const char *P_ROXIE("roxie");
static bool bDoLookUp = true;

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
            m_pStrBuffer->append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_CONFIG_2).append("check for ssh connectivity")\
                    .append(P_NAGIOS_SERVICE_CONFIG_3).append(P_CHECK_SSH_SERVICE).append(P_NAGIOS_SEPERATOR)\
                    .append(CHPCCNagiosHostEventForSSH::m_nTimeOut).append(P_NAGIOS_SERVICE_CONFIG_5);

            m_pStrBuffer->append("\n");
        }
    }
    static const int m_nTimeOut;

protected:

    CHPCCNagiosHostEventForSSH()
    {
    }
};

const int CHPCCNagiosHostEventForSSH::m_nTimeOut = 10;

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
            m_pStrBuffer->append(P_NAGIOS_HOST_CONFIG_1).append(pHostName).append(P_NAGIOS_HOST_CONFIG_2).append(pHostName).append(" ")\
                    .append(idx).append(P_NAGIOS_HOST_CONFIG_3).append(pToken).append(P_NAGIOS_HOST_CONFIG_4);
            m_pStrBuffer->append("\n");
        }
    }
protected:
    CHPCCNagiosHostEventHostConfig()
    {
    }
};

bool CHPCCNagiosToolSet::generateHostGroupsConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ALL).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine.str(), memBuff);

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    int nCount = 0;
    char pProcess[1024] = "";
    bool bAdd = false;
    char *pch = NULL;
    char pHostName[1024] = "";
    StringBuffer strHostConfig;
    int i = 0;

    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % 6 ==  0) // Process name
        {
            if (pProcess != NULL && *pProcess != 0 && strcmp(pProcess, pch) != 0)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_CONFIG_4);
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
                bDoLookUp = false;
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

            static char pLastHostName[1024] = "";

            if (bAdd == true)
            {
                strHostConfig.append(P_NAGIOS_HOSTS_GROUP_CONFIG_1).append(pProcess).append("-servers").append(P_NAGIOS_HOSTS_GROUP_CONFIG_2).append(pProcess).append(" servers")\
                                                                        .append(P_NAGIOS_HOSTS_GROUP_CONFIG_3).append(pHostName);

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

    io->write(0, strHostConfig.length(), strHostConfig.str());
    io->close();

    delete pOutput;

    return true;
}

bool CHPCCNagiosToolSet::generateServerAndHostConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 6;

    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ALL).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine, memBuff);

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    int nCount = 0;
    char pProcess[1024] = "";

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    MapIPtoNode mapIPtoHostName;

    StringBuffer strServiceConfig;

    CHPCCNagiosHostEventHostConfig evHost(&strServiceConfig);
    CHPCCNagiosToolSet::generateNagiosHostConfig(evHost, mapIPtoHostName, pEnvXML, pConfigGenPath);

    int i = -1;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && (strcmp(pch,XML_TAG_ESPPROCESS) == 0) || strcmp(pch, XML_TAG_DALISERVERPROCESS) == 0 || strcmp(pch, XML_TAG_SASHA_SERVER_PROCESS) == 0)
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
        /*else if (nCount % nNumValues == 2) // IP address
        {
            //TODO: Default check?
            /*strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(mapIPtoHostName.getValue(pch)->strHostName).append(P_NAGIOS_SERVICE_CONFIG_2).appendf("check for %s", pProcess)\
                    .append(P_NAGIOS_SERVICE_CONFIG_3).appendf("check_%s", (StringBuffer(pProcess)).toLowerCase().str()).append(P_NAGIOS_SEPERATOR).append("<TODO: PARAMS>").append(P_NAGIOS_SERVICE_CONFIG_5);

            strServiceConfig.append("\n");
        }*/

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    CHPCCNagiosToolSet::generateNagiosEspServiceConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosDaliCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosSashaCheckConfig(strServiceConfig, pEnvXML, pConfigGenPath);
    CHPCCNagiosToolSet::generateNagiosRoxieCeckConfig(strServiceConfig, pEnvXML, pConfigGenPath);

    CHPCCNagiosHostEventForSSH event(&strServiceConfig);
    MapIPtoNode map;
    CHPCCNagiosToolSet::generateNagiosHostConfig(event,map, pEnvXML, pConfigGenPath);

    io->write(0, strServiceConfig.length(), strServiceConfig.str());
    io->close();

    delete pOutput;
    return true;
}

bool CHPCCNagiosToolSet::generateNagiosHostConfig(CHPCCNagiosHostEvent &evHost, MapIPtoNode &mapIPtoHostName, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_MACHINES).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    FILE *fp = popen(strConfiggenCmdLine.str(), "r");

    if (fp == NULL)
    {
        return false;
    }

    int nCharacter = -1;
    CFileInputStream cfgInputStream(fileno(fp));

    memBuff.clear();

    do
    {
        nCharacter = cfgInputStream.readNext();

        memBuff.append(static_cast<unsigned char>(nCharacter));
    }
    while(nCharacter != -1);

    memBuff.append('\0');

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    strOutput.replace('\377',',');

    char *pOutput = strdup(strOutput.str());

    int nCount = 0;

    char *pch = NULL;
    pch = strtok(pOutput, ",\n");

    int i = 0;
    while (pch != NULL)
    {
        if (nCount % 2 ==  0) // Process name
        {
            char pHostName[1024] = "";
            struct hostent* hp = NULL;

            if (bDoLookUp == true)
            {
                unsigned int addr = inet_addr(pch);
                hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
            }

            if (hp == NULL)
            {
                bDoLookUp = false;
                strcpy(pHostName, pch);
            }
            else
            {
                strcpy(pHostName,hp->h_name);
            }

            evHost.onHostEvent(pHostName, i,pch);

            struct NodeName nm;

            nm.strHostName.set(pHostName);
            nm.strHostAlias.setf("%s %d", pHostName, i);
            mapIPtoHostName.setValue(pch, nm);

            i++;
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    delete pOutput;

    return true;
}

bool CHPCCNagiosToolSet::generateServicesConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath )
{

    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ALL).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine, memBuff);

    return true;
}

bool CHPCCNagiosToolSet::generateServicePluginConfigurationFile(const char* pOutputFilePath, const char* pEnvXML, const char* pConfigGenPath)
{
    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    return true;
}

bool CHPCCNagiosToolSet::getConfiggenOutput(const char* pEnvXML, const char* pConfigGenPath, const char* pCommandLine, MemoryBuffer &memBuff)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    FILE *fp = popen(pCommandLine, "r");

    if (fp == NULL)
    {
        return false;
    }

    int nCharacter = -1;
    CFileInputStream cfgInputStream(fileno(fp));

    memBuff.clear();

    do
    {
        nCharacter = cfgInputStream.readNext();
        memBuff.append(static_cast<unsigned char>(nCharacter));
    }
    while(nCharacter != -1);

    memBuff.append('\0');

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosEspServiceConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ESP_SERVICES).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine.str(), memBuff);

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    int i = -1;
    char pProcess[1024] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[1024] = "";
    char pServiceName[1024] = "";

    while (pch != NULL)
    {
        if (nCount % 7 ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ESPPROCESS) != 0)
            {
                delete pOutput;
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
        else if (nCount % 7 == 3) // service name
        {
            strcpy(pServiceName,pch);
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
                bDoLookUp = false;
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
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_CONFIG_2).appendf("check for %s of type %s", pServiceName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CONFIG_3).append(P_CHECK_ESP_SERVICE).append( strcmp(pch,P_HTTP) == 0 ? P_HTTP : P_HTTPS).append(P_NAGIOS_SEPERATOR)\
                    .append(strPort.str()).append(P_NAGIOS_SERVICE_CONFIG_5);

            strServiceConfig.append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }


    delete pOutput;

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosSSHCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    return true;
}

bool CHPCCNagiosToolSet::generateNagiosDaliCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ALL).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML).append(P_BY_TYPE).append(P_DALI);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine.str(), memBuff);

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    int i = -1;
    char pProcess[1024] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[1024] = "";
    char pProcessName[1024] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_DALISERVERPROCESS) != 0)
            {
                delete pOutput;
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
                bDoLookUp = false;
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
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_CONFIG_2).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CONFIG_3).append(P_CHECK_DALI).append(P_NAGIOS_SEPERATOR)\
                    .append(strPort.str()).append(P_NAGIOS_SEPERATOR).append(DALI_CHECK_TIMEOUT).append(P_NAGIOS_SERVICE_CONFIG_5);

            strServiceConfig.append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    delete pOutput;

    return true;
}

bool CHPCCNagiosToolSet::generateNagiosSashaCheckConfig(StringBuffer &strServiceConfig, const char* pEnvXML, const char* pConfigGenPath)
{
    const int nNumValues = 5;

    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ALL).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML).append(P_BY_TYPE).append(P_SASHA);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine.str(), memBuff);

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    int i = -1;
    char pProcess[1024] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[1024] = "";
    char pProcessName[1024] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_SASHA_SERVER_PROCESS) != 0)
            {
                delete pOutput;
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
                bDoLookUp = false;
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
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_CONFIG_2).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CONFIG_3).append(P_CHECK_SASHA).append(P_NAGIOS_SEPERATOR)\
                    .append(strPort.str()).append(P_NAGIOS_SEPERATOR).append(SASHA_CHECK_TIMEOUT).append(P_NAGIOS_SERVICE_CONFIG_5);

            strServiceConfig.append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    delete pOutput;

    return true;

}

bool CHPCCNagiosToolSet::generateNagiosRoxieCeckConfig(StringBuffer &strServiceConfig, const char* pEnvXML , const char* pConfigGenPath)
{
    const int nNumValues = 5;

    if (pConfigGenPath == NULL || *pConfigGenPath == 0)
    {
        pConfigGenPath = PCONFIGGEN_PATH;
    }

    if (pEnvXML == NULL || *pEnvXML == 0)
    {
        pEnvXML = PENV_XML;
    }

    if (checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(P_CONFIGGEN_PARAM_LIST_ALL).append(P_CONFIGGEN_PARAM_ENVIRONMENT).append(pEnvXML).append(P_BY_TYPE).append(P_ROXIE);

    CHPCCNagiosToolSet::getConfiggenOutput(pEnvXML, pConfigGenPath, strConfiggenCmdLine.str(), memBuff);

    StringBuffer strOutput(memBuff.toByteArray());
    strOutput.replaceString(",,",",X,"); // sttrok pecularity with adjacent delimiters
    strOutput.replaceString(",\n",",X\n"); // sttrok pecularity with adjacent delimiters
    char *pOutput = strdup(strOutput.str());

    int i = -1;
    char pProcess[1024] = "";
    int nCount = 0;
    char *pch = NULL;
    pch = strtok(pOutput, ",\n");
    StringBuffer strPort;
    StringBuffer strIPAddress;
    char pHostName[1024] = "";
    char pProcessName[1024] = "";

    while (pch != NULL)
    {
        if (nCount % nNumValues ==  0) // Process name
        {
            if (*pch != 0 && strcmp(pch, XML_TAG_ROXIE_SERVER) != 0)
            {
                delete pOutput;
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
                bDoLookUp = false;
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
            strServiceConfig.append(P_NAGIOS_SERVICE_CONFIG_1).append(pHostName).append(P_NAGIOS_SERVICE_CONFIG_2).appendf("check for %s of type %s", pProcessName, pProcess)\
                    .append(P_NAGIOS_SERVICE_CONFIG_3).append(P_CHECK_ROXIE).append(P_NAGIOS_SERVICE_CONFIG_5);

            strServiceConfig.append("\n");
        }

        pch = strtok(NULL, ",\n");

        nCount++;
    }

    delete pOutput;

    return true;
    \
}

void CHPCCNagiosToolSet::getHostName(StringBuffer &strHostName, const char *pIP, MapIPtoNode &mapIPtoHostName)
{
    if (pIP == NULL || *pIP == 0)
    {
        return;
    }


}
