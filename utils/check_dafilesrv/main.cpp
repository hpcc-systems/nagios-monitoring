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

#include "rmtfile.hpp"
#include <iostream>
#include "portlist.h"

const int nOKRetValue = 0;
const int nCriticalRetValue = 2;
const int nUnknownRetValue = 3;
const int nDefaultTimeOut=30 * 1000;
const int nExceptionRetVal = -99;
const int nDefaultPort = DAFILESRV_PORT;

void usage()
{
    std::cout << "HPCC Systems Nagios Check for Dafilesrv\nUsage: check_dafilesrv <dafilesrv IP> <Port Default = 7100> \n";
}

int critical(const char* pIP, int nPort, int nDafilesrvRetVal)
{
    StringBuffer strMsg;
    if (nDafilesrvRetVal != nExceptionRetVal)
    {
        strMsg.setf("CRITICAL - Can not connect to dafilesrv %s:%d\nwith error val %d\n", pIP, nPort, nDafilesrvRetVal);
    }
    else
    {
        strMsg.setf("CRITICAL - Can not connect to dafilesrv %s:%d\nwith exception thrown on connect\n", pIP, nPort);
    }
    std::cout << strMsg.str();

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

    int nPort = nDefaultPort;

    if (argc != 2 && argc != 3)
    {
        usage();
        return nUnknownRetValue;
    }

    if (argc == 3)
    {
        nPort = atoi(argv[2]);
    }

    SocketEndpoint ep(argv[1], nPort);

    StringBuffer strRetStr;
    int nRetVal = nExceptionRetVal;

    try
    {
        nRetVal = getDafileSvrInfo(ep, 0, strRetStr);
    }
    catch(IException *)
    {
        return critical(argv[1], nPort, nRetVal);
    }

    if (nRetVal == 0)
    {
        return ok();
    }
    else
    {
        return critical(argv[1], nPort, nRetVal);
    }
}
