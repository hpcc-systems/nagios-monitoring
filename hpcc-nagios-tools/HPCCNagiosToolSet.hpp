#ifndef _HPCC_NAGIOSTOOLSET_HPP_
#define _HPCC_NAGIOSTOOLSET_HPP_

static const char *PCONFIGGEN_PATH("/opt/HPCCSystems/sbin/configgen");

class StringArray;

class CHPCCNagiosToolSet
{
public:
    bool generateHostGroupFile(const char* pOutputFilePath, const char* pConfigGenPath = PCONFIGGEN_PATH);
    bool generateServiceDefinitionFile(const char* pOutputFilePath, const char* pConfigGenPath = PCONFIGGEN_PATH);

protected:

    //void createServiceDefinitionString(const char)
    void createHostGroupString(StringArray &pIP, StringBuffer &strHostGroup);
};

#endif // _HPCC_NAGIOSTOOLSET_HPP_
