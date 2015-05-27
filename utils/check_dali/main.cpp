/*##############################################################################

    HPCC SYSTEMS software Copyright (C) 2015 HPCC Systems.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
############################################################################## */

#include "daclient.hpp"
#include "jsocket.hpp"
#include "jstring.hpp"
#include "mpbase.hpp"
#include "jlog.hpp"
#include <iostream>

const int nOKRetValue = 0;
const int nCriticalRetValue = 2;
const int nDefaultTimeOut=30 * 1000;

void usage()
{
    std::cout << "HPCC Systems Nagios Check for Dali\nUsage: check_dali <dali IP> <Port> [Timeout default = " << nDefaultTimeOut << "]\n";
}

int critical(const char* pIP, const char* pPort)
{
    StringBuffer strMsg;
    strMsg.setf("CRITICAL - Can not connect to dali %s:%s\n", pIP, pPort);
    std::cout << strMsg.str();
    sleep(10);  //let thread finish
    return nCriticalRetValue;
}

int ok()
{
    std::cout << "OK\n";
    return nOKRetValue;
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

        closedownClientProcess();
        return critical(argv[1], argv[2]);
    }

    try
    {
        Owned<IRemoteConnection> conn = querySDS().connect("./", myProcessSession(), RTM_LOCK_READ, nTimeOut);

        if (conn == NULL)
        {
            closedownClientProcess();
            return critical(argv[1], argv[2]);
        }
        else
        {
            closedownClientProcess();
            return ok();
        }
    }
    catch (IException *)
    {
        closedownClientProcess();
        return critical(argv[1], argv[2]);
    }

    return 0;
}
