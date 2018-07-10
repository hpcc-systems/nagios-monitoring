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

#include <iostream>
#include "build-config.h"
#include "jlib.hpp"
#include "jsocket.hpp"
#include "sacmd.hpp"
#include "mpbase.hpp"
#include "mpcomm.hpp"

const int nOKRetValue = 0;
const int nCriticalRetValue = 2;
const int nUnknownRetValue = 3;
const int nDefaultTimeOut=30 * 1000;

void usage()
{
    std::cout << "HPCC Systems Nagios Check for Sasha\nUsage: check_sasha <sasha> <Port> [Timeout default = " << nDefaultTimeOut << "]\n";
}

int critical(const char* pIP, const char* pPort)
{
    StringBuffer strMsg;
    strMsg.setf("CRITICAL - Can not connect to sasha %s:%s\n", pIP, pPort);
    std::cout << strMsg.str();

    stopMPServer();

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
        return nUnknownRetValue;
    }

    int nTimeOut = (argc == 4 ? atoi(argv[3]) : nDefaultTimeOut);

    startMPServer(0);
    Owned<ISashaCommand> cmd = createSashaCommand();
    cmd->setAction(SCA_null);


    SocketEndpoint ep(argv[1], atoi(argv[2]));
    Owned<INode> node = createINode(ep);

    try
    {
        if (cmd->send(node, nTimeOut) == false)
        {
            return critical(argv[1], argv[2]);
        }
        else
        {
            return ok();
        }
    }

    catch (...)
    {
        return critical(argv[1], argv[2]);
    }

    stopMPServer();
    return 0;
}
