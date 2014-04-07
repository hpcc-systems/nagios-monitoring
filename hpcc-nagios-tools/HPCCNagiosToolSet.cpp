#include <cstdio>
#include "jstream.ipp"
#include "jfile.hpp"
#include "jbuff.hpp"
#include "HPCCNagiosToolSet.hpp"

static const char *PCONFIGGEN_PARAM_LIST(" -list ");
static const char *PCONFIGGEN_PARAM_LIST_ALL(" -listall ");
static const char *PCONFIGGEN_PARAM_ENVIRONMENT(" -env ");
static const char *PCONFIGGEN_PARAM_MACHINES(" -machines ");

bool CHPCCNagiosToolSet::generateHostGroupFile(const char* pOutputFilePath, const char* pConfigGenPath)
{

}

bool CHPCCNagiosToolSet::generateServiceDefinitionFile(const char* pOutputFilePath, const char* pConfigGenPath)
{
    if (pOutputFilePath == NULL || *pOutputFilePath == 0 || pConfigGenPath == NULL || *pConfigGenPath == 0 || checkFileExists(pOutputFilePath) == false || checkFileExists(pConfigGenPath) == false)
    {
        return false;
    }

    MemoryBuffer memBuff;
    StringBuffer strConfiggenCmdLine(pConfigGenPath);

    strConfiggenCmdLine.append(PCONFIGGEN_PARAM_MACHINES);

    FILE *fp = popen(strConfiggenCmdLine.str());

    if (fp == NULL)
    {
        return false;
    }

    int nCharacter = -1;
    CFileInputStream cfgInputStream(fileno(fp));

    memBuff->clear();

    do
    {
        nCharacter = cfgInputStream.readNext();

        memBuff->append(static_cast<unsigned char>(nCharacter));
    }
    while(nCharacter != -1);

    memBuff->append('\0');

    OwnedIFile outputFile = createIFile(pOutputFilePath);
    OwnedIFileIO io = outputFile->open(IFOcreaterw);

    if (io == NULL)
    {
        return false;
    }

    StringBuffer strOutput(memBuff.toByteArray());

    char *pch = NULL;

    pch = strtok(strOutput.str(), ",\n");

    int nCount = 0;

    StringArray arrProcessTypes;
    MapStringTo<StringArray> mapProcessToNodes;

    const char pProcess[1024] = "";

    while (pch != NULL)
    {
        if (nCount % 5 ==  0) // Process name
        {
            strncpy(pProcess, pch, sizeof(pProcess));

        }
        else if (nCount % 5 == 2) // IP address
        {
            StringArray *pArrIPList = mapProcessToNodes.getValue(pProcess);

            if (pArrIPList == NULL)
            {
                pArrIPList = new StringArray();
                arrProcessTypes.append(pProcess);
                mapProcessToNodes.setValue(pProcess, pArrIPList);
            }

            pArrIPList->append(pch);
        }

        pch = strtok(strOutput.str(), ",\n");

        nCount++;
    }

    for (int n = 0; n < arrProcessTypes.length(); n++)
    {
        //const char *pl
        delete &(arrProcessTypes.item(n));
    }
}

void CHPCCNagiosToolSet::createHostGroupString(StringArray &pIP, StringBuffer &strHostGroup)
{

}
