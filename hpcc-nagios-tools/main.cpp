#include "HPCCNagiosToolSet.hpp"
#include "jstring.hpp"

int main(int argc, char *argv[])
{
    if (stricmp(argv[1], "-o") == 0 || stricmp(argv[1], "-host") == 0)
    {
        if (argc == 3)
        {
            CHPCCNagiosToolSet::generateHostGroupFile(const_cast<const char*>(argv[2]), const_cast<const char*>(argv[3]));
        }
        else
        {
            StringBuffer strOutput;
            CHPCCNagiosToolSet::generateHostGroupFile(strOutput);
        }
    }
    else if (stricmp(argv[1], "-s") == 0 || stricmp(argv[1], "-service") == 0)
    {
        if (argc == 3)
        {
            CHPCCNagiosToolSet::generateServiceDefinitionFile(const_cast<const char*>(argv[2]));//, const_cast<const char*>(argv[3]));
        }
        else
        {
            StringBuffer strOutput;
            CHPCCNagiosToolSet::generateServiceDefinitionFile(strOutput);
        }

    }
    return 0;
}
