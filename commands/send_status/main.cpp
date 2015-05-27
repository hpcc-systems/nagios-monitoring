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
#include "jstring.hpp"
#include "componentstatus.hpp"
#include "ws_machine.hpp"

/*
define command{
        command_name    send_service_status
        command_line    /usr/lib/nagios/plugins/send_status -o $HOSTADDRESS$ -s $SERVICESTATE$ -d '$SERVICENOTES$' -t $TIMET$ -n $SERVICEDISPLAYNAME$ -e 10.176.152.186:8010
}

*/

void usage()
{
    std::cout << "HPCC Systems EclWatch Monitoring Client\n";
    std::cout << "\
Usage: send_status -o <host:port> -s <state> -e <host/WsMachine:port>  -n <name> [-d <notes/detail>] [-t <time> [-u <username>] [-p <password>] [-l] [-r] \n";
    std::cout << "\
Example: \"send_status -h 10.0.0.1 -s OK -e 192.168.1.1:8010 -n DaliServerProcess -d \"System Down\" -t  1435067145 -l \n";
    std::cout << " -e      : eclwatch host:port (multiple instances allowed) \n";
    std::cout << " -o      : host[:port] of checked node\n";
    std::cout << " -s      : state (e.g. 'OK' 'CRITICAL' 'WARNING' 'UNKNWON)\n";
    std::cout << " -d      : note - status detail\n";
    std::cout << " -t      : time since epoch\n";
    std::cout << " -n      : component name (e.g. DaliServerProcess)\n";
    std::cout << " -r      : detail url\n";
    std::cout << " -u      : username for ldap enabled esps (requires password option)\n";
    std::cout << " -p      : password for ldap enabled esps (requires username option)\n";
    std::cout << " -l      : use https (HIGHLY RECOMMENDED when using username/password)\n";
    std::cout << " -v      : verbose\n";
    std::cout << " -g      : when available use host and port from detail (assumes ip:<host>:<port> is end of string in detail)\n";
    std::cout << " -h      : help\n";
}

int main(int argc, char* argv[])
{
    if (argc == 1 || stricmp(argv[1],"-h") == 0 || stricmp(argv[1],"-help") == 0)
    {
        usage();
        return 0;
    }

    InitModuleObjects();

    int i = 1;
    StringBuffer strUrl;
    bool bVerbose = false;
    StringBuffer strState, strNote, strTime, strComponenetName, strUserName, strPassword, strURL, strHost;
    StringArray strEclWatchHostPortArray;
    bool bUseHTTPS = false;
    bool bAppendPortFromDetail = false;

    while (i < argc)
    {
        if (stricmp(argv[i], "-e") == 0)
        {
            i++;
            strEclWatchHostPortArray.append(argv[i]);
        }
        else if (stricmp(argv[i], "-v") == 0)
        {
            bVerbose = true;
        }
        else if (stricmp(argv[i], "-s") == 0)
        {
            i++;
            strState.set(argv[i]);
        }
        else if (stricmp(argv[i], "-d") == 0)
        {
            i++;
            strNote.set(argv[i]);
        }
        else if (stricmp(argv[i], "-t") == 0)
        {
            i++;
            strTime.set(argv[i]);
        }
        else if (stricmp(argv[i], "-n") == 0)
        {
            i++;
            strComponenetName.set(argv[i]);
        }
        else if (stricmp(argv[i], "-u") == 0)
        {
            i++;
            strUserName.set(argv[i]);
        }
        else if (stricmp(argv[i], "-p") == 0)
        {
            i++;
            strPassword.set(argv[i]);
        }
        else if (stricmp(argv[i], "-d") == 0)
        {
            i++;
            strNote.set(argv[i]);
        }
        else if (stricmp(argv[i], "-l") == 0)
        {
            bUseHTTPS = true;
        }
        else if (stricmp(argv[i], "-o") == 0)
        {
            i++;
            strHost.set(argv[i]);
        }
        else if (stricmp(argv[i], "-r") == 0)
        {
            i++;
            strURL.set(argv[i]);
        }
        else if (stricmp(argv[i], "-g") == 0)
        {
            bAppendPortFromDetail = true;
        }
        else if (stricmp(argv[i], "-h") == 0)
        {
            usage();
            return 0;
        }
        i++;
    }

    if (strEclWatchHostPortArray.length() == 0)
    {
        std::cout << "Error: no eclwatch host and port supplied!\n";
        exit(1);
    }
    if (strState.length() == 0)
    {
        std::cout << "No state specified!\n";
        exit(1);
    }
    if (strTime.length() == 0)
    {
        std::cout << "No epoch time specified!\n";
        exit(1);
    }
    if (strHost.length() == 0)
    {
        std::cout << "No name specified!\n";
        exit(1);
    }
    if (strNote.str() > 0 && bAppendPortFromDetail == true)
    {
        const char *pPort = strstr(strNote.str(), "ip:");

        if (pPort != NULL)
        {
            strHost.set((&pPort[3]));
        }
    }
    if (strUserName.length() ^ strPassword.length())
    {
        std::cout << "Username and pasword need to be specified!\n";
    }

    if (bVerbose == true)
    {
        std::cout << "host: '" << strHost.str() << "'" << std::endl;
        std::cout << "url: '" << strURL.str() << "'" << std::endl;
        std::cout << "password: '" << strPassword.str() << "'" << std::endl;
        std::cout << "username: '" << strUserName.str() << "'" << std::endl;
        std::cout << "component name: '" << strComponenetName.str() << "'" << std::endl;
        std::cout << "time: '" << strTime.str() << "'" << std::endl;
        std::cout << "state: '" << strState.str() << "'" << std::endl;
    }

    Owned<IClientws_machine> pMachine = createws_machineClient();

    if (strPassword.length () > 0)
    {
        pMachine->setUsernameToken(strUserName.str(), strPassword.str(), NULL);
    }

    int idx = 0;

    for (idx = 0; idx < strEclWatchHostPortArray.length(); idx++)
    {
        strUrl.clear();

        strUrl.set(bUseHTTPS == true ? "https://" : "http://");
        strUrl.append(strEclWatchHostPortArray.item(idx));
        strUrl.append("/ws_machine");

        IArrayOf<IConstComponentStatus> statusList;
        Owned<IEspComponentStatus> cs1 = createComponentStatus();

        cs1->setEndPoint(strHost.str());
        cs1->setComponentType(strComponenetName.str());
        cs1->setReporter("nagios");


        IArrayOf<IConstStatusReport> statusReports1;
        Owned<IEspStatusReport> statusReport1 = createStatusReport();


        statusReport1->setStatus(strcmp(strState.str(),"CRITICAL") == 0 ? "Error" : (strcmp(strState.str(),"OK") == 0 ? "Normal" : "Warning" ));
        statusReport1->setURL(strURL.str());
        statusReport1->setStatusDetails(strNote.str());
        statusReport1->setTimeReported(atoi(strTime.str()));

        statusReport1->setStatusDetails(strNote.str());
        statusReports1.append(*statusReport1.getClear());
        cs1->setStatusReports(statusReports1);
        statusList.append(*cs1.getClear());

        Owned<IClientUpdateComponentStatusRequest> req = createClientUpdateComponentStatusRequest();

        req->setReporter("nagios");
        req->setComponentStatusList(statusList);

        pMachine->addServiceUrl(strUrl.str());
        pMachine->UpdateComponentStatus(req);


    }
}
