#include "daclient.hpp"
#include "jsocket.hpp"
#include "jstring.hpp"
#include "mpbase.hpp"
#include "jlog.hpp"
#include <iostream>

const int nDefaultTimeOut=30 * 1000;

void usage()
{
    std::cout << "HPCC Systems Nagios Check for Dali\nUsage: check_dali <dali IP> <Port> [Timeout default = " << nDefaultTimeOut << "]\n";
}

int critical(const char* pIP, const char* pPort)
{
    StringBuffer strMsg;
    strMsg.setf("CRITICAL | Can not connect to to dali %s:%s\n", pIP, pPort);
    std::cout << strMsg.str();

    return 3;
}


int ok()
{
    std::cout << "OK\n";
    return 0;
}

int main(int argc, char* argv[])
{
    InitModuleObjects();

    if (argc != 3 && argc != 4)
    {
        usage();
        return 0;
    }

    int nTimeOut = (argc == 4 ? atoi(argv[3]) : nDefaultTimeOut);

    SocketEndpoint ep(argv[1], atoi(argv[2]));
    SocketEndpointArray epa;

    epa.append(ep);

    IGroup *group = createIGroup(epa);

    try
    {
        initClientProcess(group, DCR_Other, 0, 0, 0, nTimeOut);
    }
    catch(IException *)
    {
        return critical(argv[1], argv[2]);
    }

    try
    {
        Owned<IRemoteConnection> conn = querySDS().connect("./", myProcessSession(), RTM_LOCK_READ, nTimeOut);

        if (conn == NULL)
        {
            return critical(argv[1], argv[2]);
        }
        else
        {
            return ok();
        }
    }
    catch (IException *)
    {
        return critical(argv[1], argv[2]);
    }

    return 0;
}
