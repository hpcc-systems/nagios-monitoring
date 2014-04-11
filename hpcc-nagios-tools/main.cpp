#include "HPCCNagiosToolSet.hpp"
#include "jstring.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc == 1 || stricmp(argv[1],"-h") == 0 || stricmp(argv[1],"-help") == 0)
    {
        std::cout << "Help";
    }
    else if (stricmp(argv[1], "-o") == 0 || stricmp(argv[1], "-host") == 0)
    {
        if (argc == 4)
        {
            CHPCCNagiosToolSet::generateHostGroupsFile(const_cast<const char*>(argv[2]), const_cast<const char*>(argv[3]));
        }
        else
        {
            StringBuffer strOutput;
            CHPCCNagiosToolSet::generateHostGroupsFile(strOutput);
        }
    }
    else if (stricmp(argv[1], "-s") == 0 || stricmp(argv[1], "-service") == 0)
    {
        if (argc == 4)
        {
            CHPCCNagiosToolSet::generateServiceDefinitionFile(const_cast<const char*>(argv[3]), const_cast<const char*>(argv[2]));//, const_cast<const char*>(argv[3]));
        }
        else
        {
            StringBuffer strOutput;
            CHPCCNagiosToolSet::generateServiceDefinitionFile(strOutput);
        }

    }

    return 0;
}
