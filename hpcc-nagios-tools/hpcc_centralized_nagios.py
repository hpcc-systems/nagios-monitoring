#!/usr/bin/env python3

##############################################################################
#    HPCC SYSTEMS software Copyright (C) 2015 HPCC Systems.
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#       http://www.apache.org/licenses/LICENSE-2.0
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
###############################################################################


import lxml.etree as ET
import subprocess
import paramiko
import getpass
import sys
import os

Environment = input('Enter Environment Name > ')
Address = input('Enter IP > ')
ProdOrNot = input(
    'Please specify if this environment is production by typing prod or if it is a non-production by typing non-prod > ')
xxx = getpass.getpass()
user = input('Enter username > ')
xml_local = '/tmp/environment.xml'
xml_remote = '/etc/HPCCSystems/environment.xml'

if ProdOrNot == 'prod':
    HostDefinition = 'generic-prod-hpcc'
    ServiceDefinition = 'hpcc-prod-service'
elif ProdOrNot == 'non-prod':
    HostDefinition = 'generic-nonprod-hpcc'
    ServiceDefinition = 'hpcc-nonprod-service'
else:
    print('Invalid Option')
    sys.exit(1)


class XML():
    def __init__(self):
        self.xml = self

    def getXML(self):
        host = paramiko.Transport((Address, 22))
        host.connect(username=user, password=xxx)
        sftp = paramiko.SFTPClient.from_transport(host)
        sftp.get(xml_remote, xml_local)


class XMLParser():

    def __init__(self):
        self.parsing = self

    def hpcc_components(self, component_name):
        ParseXML = ET.parse(xml_local)
        doc_root = ParseXML.getroot()
        cluster = []
        for i in doc_root.findall('.//{}Process/Instance'.format(component_name)):
            ip = i.attrib['netAddress']
            cluster.append(ip)
        return cluster

    def hpcc_master_components(self, component_name):
        ParseXML = ET.parse(xml_local)
        doc_root = ParseXML.getroot()
        ip_port = []
        for i in doc_root.findall('.//{}ServerProcess/Instance'.format(component_name)):
            master_component = i.attrib['netAddress']
            ip_port.append(master_component)
        for i in doc_root.findall('.//{}ServerProcess/Instance'.format(component_name)):
            port = i.attrib['port']
            ip_port.append(port)
        return ip_port

    def Esp(self):
        ParseXML = ET.parse(xml_local)
        doc_root = ParseXML.getroot()
        ip_port = {}
        ports = []
        for i in doc_root.findall('.//EspProcess/Instance'):
            ip = i.attrib['netAddress']
            for i in doc_root.findall('.//EspProcess/EspBinding'):
                port = i.attrib['port']
                ports.append(port)
            ip_port[ip] = ports
        return ip_port

    def Roxie(self):
        ParseXML = ET.parse(xml_local)
        doc_root = ParseXML.getroot()
        Roxies = []
        for roxie in doc_root.findall('.//RoxieServerProcess'):
            Roxie = roxie.attrib['computer']
            Roxies.append(Roxie)
        ip_addr = {}
        for node in doc_root.findall('.//Hardware/Computer'):
            Machine = node.attrib['name']
            IP = node.attrib['netAddress']
            ip_addr[Machine] = IP
        RoxieSlaves = []
        for key in ip_addr:
            if key not in Roxies:
                continue
            elif key in Roxies:
                RoxieSlaves.append(ip_addr[key])
        return RoxieSlaves

    def ThorMaster(self):
        ParseXML = ET.parse(xml_local)
        doc_root = ParseXML.getroot()
        Thors = []
        for i in doc_root.findall('.//ThorMasterProcess'):
            Thor = i.attrib['computer']
            Thors.append(Thor)
        d = {}
        for i in doc_root.findall('.//Hardware/Computer'):
            Machine = i.attrib['name']
            IP = i.attrib['netAddress']
            d[Machine] = IP
        masterport = {}
        machine = []
        for i in doc_root.findall('.//ThorCluster'):
            try:
                Computer = i.attrib['computer']
                Port = i.attrib['masterport']
                if Computer in d:
                    machine.append(Port)
            except KeyError:
                continue
            masterport.setdefault(d[Computer], [])
            masterport[d[Computer]].append(machine)
            machine = []
        ThorMasters = []
        for i in d:
            if i not in Thors:
                continue
            elif i in Thors:
                ThorMasters.append(d[i])
        return ThorMasters, masterport

    def ThorSlave(self):
        ParseXML = ET.parse(xml_local)
        doc_root = ParseXML.getroot()
        Thors = []
        for i in doc_root.findall('.//ThorSlaveProcess'):
            Thor = i.attrib['computer']
            Thors.append(Thor)
        d = {}
        for i in doc_root.findall('.//Hardware/Computer'):
            Machine = i.attrib['name']
            IP = i.attrib['netAddress']
            d[Machine] = IP
        ThorSlaves = []
        for i in d:
            if i not in Thors:
                continue
            elif i in Thors:
                ThorSlaves.append(d[i])
        return ThorSlaves

XML().getXML()
hpcc_component = XMLParser()

# List of HPCC Components from  xml file.
dafilesrv = hpcc_component.hpcc_components('Dafilesrv')
dali = hpcc_component.hpcc_master_components('Dali')
sasha = hpcc_component.hpcc_master_components('Sasha')
eclagent = hpcc_component.hpcc_components('EclAgent')
eclccserver = hpcc_component.hpcc_components('EclccServer')
eclserver = hpcc_component.hpcc_components('EclServer')
dfuserver = hpcc_component.hpcc_components('DfuServer')
eclscheduler = hpcc_component.hpcc_components('EclScheduler')
esp = hpcc_component.Esp()
roxie = hpcc_component.Roxie()
thormaster = hpcc_component.ThorMaster()
thorslave = hpcc_component.ThorSlave()

print('=== list of IP Addresses ==='), '\n', dafilesrv, '\n'
print('Number of servers = '), len(dafilesrv), '\n'
print('=========================== Main Components =======================', '\n')
print('dali = '), dali, '\n'
print('sasha = '), sasha, '\n'
print('eclagent = '), eclagent, '\n'
print('Number of ECLAgents = '), len(eclagent), '\n'
print('eclccserver = ', eclccserver, '\n')
print('Number of eclccservers = '), len(eclccserver), '\n'
print('eclserver = '), eclserver, '\n'
print('Number of eclservers = '), len(eclserver), '\n'
print('dfuserver = ', dfuserver, '\n')
print('Number of DFUServers = '), len(dfuserver), '\n'
print('eclscheduler = '), eclscheduler, '\n'
print('Number of schedulers = '), len(eclscheduler), '\n'
print('esp = '), esp, '\n'
print('Number of ESP Servers = '), len(esp), '\n'
print('Roxie = '), roxie, '\n'
print('Number of Roxie Servers = '), len(roxie), '\n'
print('ThorMaster = ', thormaster[0], '\n')
print('Number of ThorMaster Nodes = '), len(thormaster[0]), '\n'
if len(thormaster[1]) > 0:
    print('Port Layout = '), thormaster[1], '\n'
print('ThorSlaves = '), thorslave, '\n'
print('Number of ThorSlaves = '), len(thorslave), '\n'
print('======================================================================', '\n')

### Creating directory structure

EnvironmentDirectory = '/etc/HPCCSystems/nagios/{}'.format(Environment)

if os.path.isdir(EnvironmentDirectory) == False:
    subprocess.call('mkdir -p /etc/HPCCSystems/nagios/{}'.format(Environment), shell=True)
else:
    print('Environment already exists')
    sys.exit(1)

### Generating host configuration files with basic checks

for ip in dafilesrv:
    if ip:
        filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, ip)
        config_file = open(filename, 'w')
        host_conf = """

define host {{
        use                 {0}            
        host_name           {1}
        alias               {1}
        address             {1}
        }}

define service {{
	use                  {2}
	host_name            {1}
	service_description  Dafilesrv Status
	check_command        check_dafilesrv
}}

define service {{
	use                  {2}
	host_name            {1}
	service_description  SSH Status 
	check_command        check_ssh
}}


""".format(HostDefinition, ip, ServiceDefinition)
        config_file.write(host_conf)
        config_file.close()


### Appending Dali entry to host configuration file

def InsertDaliEntry():
    filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, dali[0])
    config_file = open(filename, 'a')
    host_conf = """

define service {{
    use                  {}
    host_name            {}
    service_description  Dali Service Status
    check_command        check_dali!{}!5000
}}

""".format(ServiceDefinition, dali[0], dali[1])
    config_file.write(host_conf)
    config_file.close()


### Appending Sasha entry to host configuration file

def InsertSashaEntry():
    filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, sasha[0])
    config_file = open(filename, 'a')
    host_conf = """

define service {{
    use                  {}
    host_name            {}
    service_description  Sasha Service Status
    check_command        check_sasha!{}!5000
}}

""".format(ServiceDefinition, sasha[0], sasha[1])
    config_file.write(host_conf)
    config_file.close()


### Appending ESP entry to host configuration file

def InsertESPServerEntry():
    for ip in esp:
        filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, ip)
        config_file = open(filename, 'a')
        for port in esp[ip]:
            host_conf = """

define service {{
    use                  {0}
    host_name            {1}
    service_description  ESP Status port {2}
    check_command        check_esp!{2}
}}

""".format(ServiceDefinition, ip, port)
            config_file.write(host_conf)
        config_file.close()


### Appending Roxie entry to host configuration file

def InsertRoxieEntry():
    if roxie:
        for ip in roxie:
            filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, ip)
            config_file = open(filename, 'a')
            host_conf = """

define service {{
    use                  {}
    host_name            {}
    service_description  Roxie Status
    check_command        check_roxie
}}


""".format(ServiceDefinition, ip)
            config_file.write(host_conf)
            config_file.close()


### Appending ThorMaster entry to host configuration file

def InsertThorMasterEntry():
    if thormaster:
        for ip in thormaster[0]:
            if len(thormaster[1]) > 0:
                for port in thormaster[1][ip]:
                    filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, ip)
                    config_file = open(filename, 'a')
                    host_conf = """

define service {{
    use                  {0}
    host_name            {1}
    service_description  ThorMaster Status port {2}
    check_command        check_thormaster!{2}
}}

""".format(ServiceDefinition, ip, int(port[0]))
                    config_file.write(host_conf)
                    config_file.close()
            else:
                filename = '/etc/HPCCSystems/nagios/{}/{}.cfg'.format(Environment, ip)
                config_file = open(filename, 'a')
                host_conf = """

define service {{
    use                  {}
    host_name            {}
    service_description  ThorMaster Status port 20000
    check_command        check_thormaster!20000
}}

""".format(ServiceDefinition, ip)
                config_file.write(host_conf)
                config_file.close()


### Appending new entries to hostgroup file

def EditHostGroups():
    hostgroup_filename = open('/etc/HPCCSystems/nagios/{0}/{0}_hostgroup.cfg'.format(Environment), 'a')
    cleanup = ','.join(dafilesrv)
    bulk = """
define hostgroup {{
        hostgroup_name  {0}
        alias           {0}
        members         {1}
}}
""".format(Environment, cleanup)
    hostgroup_filename.write(bulk)
    hostgroup_filename.close()


def hpccCommands():
    filename = '/etc/HPCCSystems/nagios/hpcc_objects/hpcc_commands.cfg'
    if os.path.isfile(filename) == False:
        config_file = open(filename, 'a')

        command_conf = """


### HPCC Platform Checks ###

define command{
        command_name  check_dafilesrv
        command_line  /usr/lib/nagios/plugins/check_dafilesrv $HOSTADDRESS$
        }
define command{
        command_name  check_dali
        command_line  /usr/lib/nagios/plugins/check_dali $HOSTADDRESS$ $ARG1$ $ARG2$
        }
define command{
        command_name  check_roxie
        command_line  /usr/lib/nagios/plugins/check_roxie $HOSTADDRESS$
        }
define command{
        command_name  check_sasha
        command_line  /usr/lib/nagios/plugins/check_sasha $HOSTADDRESS$ $ARG1$ $ARG2$
        }


### HPCC Custom Checks ###

define command{
        command_name  check_esp
        command_line  /usr/local/nagios/libexec/check_tcp -H $HOSTADDRESS$ -p $ARG1$
        }
define command{
        command_name  check_thormaster
        command_line  /usr/local/nagios/libexec/check_tcp -H $HOSTADDRESS$ -p $ARG1$
        }

"""
        config_file.write(command_conf)
        config_file.close()


def hpccTimePeriods():
    filename = '/etc/HPCCSystems/nagios/hpcc_objects/hpcc_timeperiods.cfg'
    if os.path.isfile(filename) == False:
        config_file = open(filename, 'a')
        definitions_conf = """

# This defines a timeperiod where all times are valid for checks, 
# notifications, etc.  The classic "24x7" support nightmare. :-)
define timeperiod{
        timeperiod_name hpcc-24x7
        alias           24 Hours A Day, 7 Days A Week
        sunday          00:00-24:00
        monday          00:00-24:00
        tuesday         00:00-24:00
        wednesday       00:00-24:00
        thursday        00:00-24:00
        friday          00:00-24:00
        saturday        00:00-24:00
        }

# 'workhours/banker hours' timeperiod definition for non-production systems
define timeperiod{
        timeperiod_name hpcc-workhours
        alias           Normal Work Hours
        monday          06:00-17:00
        tuesday         06:00-17:00
        wednesday       06:00-17:00
        thursday        06:00-17:00
        friday          06:00-17:00
        }
"""
        config_file.write(definitions_conf)
        config_file.close()


def hpccContacts():
    filename = '/etc/HPCCSystems/nagios/hpcc_objects/hpcc_contacts.cfg'
    if os.path.isfile(filename) == False:
        config_file = open(filename, 'a')
        definitions_conf = """
define contact{
        contact_name                    hpcc_support_team             ; Short name of user
        use                             generic-contact               ; Inherit default values from generic-contact template
        alias                                                         ; Full name of user

        email                           somedistributionlist@someplace.com ; <<***** CHANGE THIS TO YOUR EMAIL ADDRESS ******
        }

define contactgroup{
        contactgroup_name       hpcc-admins
        alias                   hpcc-administrators
        members                 hpcc_support_team
        }
"""
        config_file.write(definitions_conf)
        config_file.close()


def hpccProdDefinitions():
    filename = '/etc/HPCCSystems/nagios/hpcc_objects/hpcc_prod_definitions.cfg'
    if os.path.isfile(filename) == False:
        config_file = open(filename, 'a')
        definitions_conf = """

# HPCC Host Definition
define host{
        name                            generic-prod-hpcc    ; The name of this host template
        notifications_enabled           1               ; Host notifications are enabled
        event_handler_enabled           1               ; Host event handler is enabled
        flap_detection_enabled          1               ; Flap detection is enabled
        process_perf_data               1               ; Process performance data
        retain_status_information       1               ; Retain status information across program restarts
        retain_nonstatus_information    1               ; Retain non-status information across program restarts
        notification_period             hpcc-24x7            ; Send host notifications at any time
	notification_interval   	30              ; Resend notifications every 30 minutes
        notification_options    	d,r             ; Only send notifications for specific host states
	contact_groups          	hpcc-admins          ; Notifications get sent to the admins by default
	check_period            	hpcc-24x7            ; By default, switches are monitored round the clock
	check_interval         		m5               ; Switches are checked every 5 minutes
        retry_interval          	1               ; Schedule host check retries at 1 minute intervals
        max_check_attempts      	10              ; Check each switch 10 times (max)
        check_command           check-host-alive        ; Default command to check if routers are "alive"
        register                        0               ; DONT REGISTER THIS DEFINITION - ITS NOT A REAL HOST, JUST A TEMPLATE!
        }

# HPCC Service Definition
define service{
        name                            hpcc-prod-service         ; The 'name' of this service template
        active_checks_enabled           1                       ; Active service checks are enabled
        passive_checks_enabled          1                       ; Passive service checks are enabled/accepted
        parallelize_check               1                       ; Active service checks should be parallelized (disabling this can lead to major performance problems)
        obsess_over_service             1                       ; We should obsess over this service (if necessary)
        check_freshness                 0                       ; Default is to NOT check service 'freshness'
        notifications_enabled           1                       ; Service notifications are enabled
        event_handler_enabled           1                       ; Service event handler is enabled
        flap_detection_enabled          1                       ; Flap detection is enabled
        process_perf_data               1                       ; Process performance data
        retain_status_information       1                       ; Retain status information across program restarts
        retain_nonstatus_information    1                       ; Retain non-status information across program restarts
        is_volatile                     0                       ; The service is not volatile
        check_period                    hpcc-24x7               ; The service can be checked at any time of the day
        max_check_attempts              3                       ; Re-check the service up to 3 times in order to determine its final (hard) state
        normal_check_interval           10                      ; Check the service every 10 minutes under normal conditions
        retry_check_interval            2                       ; Re-check the service every two minutes until a hard state can be determined
        contact_groups                  hpcc-admins                  ; Notifications get sent out to everyone in the 'admins' group
        notification_options            w,u,c,r                 ; Send notifications about warning, unknown, critical, and recovery events
        notification_interval           60                      ; Re-notify about service problems every hour
        notification_period             hpcc-24x7               ; Notifications can be sent out at any time
         register                        0                      ; DONT REGISTER THIS DEFINITION - ITS NOT A REAL SERVICE, JUST A TEMPLATE!
        }

"""
        config_file.write(definitions_conf)
        config_file.close()


def hpccNonProdDefinitions():
    filename = '/etc/HPCCSystems/nagios/hpcc_objects/hpcc_nonprod_definitions.cfg'
    if os.path.isfile(filename) == False:
        config_file = open(filename, 'a')
        definitions_conf = """

# HPCC Host Definition
define host{
        name                            generic-nonprod-hpcc    ; The name of this host template
        notifications_enabled           1                       ; Host notifications are enabled
        event_handler_enabled           1                       ; Host event handler is enabled
        flap_detection_enabled          1                       ; Flap detection is enabled
        process_perf_data               1                       ; Process performance data
        retain_status_information       1                       ; Retain status information across program restarts
        retain_nonstatus_information    1                       ; Retain non-status information across program restarts
        notification_period             hpcc-workhours          ; Send host notifications at any time
	    notification_interval   	30                      ; Resend notifications every 30 minutes
        notification_options    	d,r                     ; Only send notifications for specific host states
	    contact_groups          	hpcc-admins                  ; Notifications get sent to the admins by default
	check_period            	hpcc-24x7               ; By default, switches are monitored round the clock
	check_interval         		m5                      ; Switches are checked every 5 minutes
        retry_interval          	1                       ; Schedule host check retries at 1 minute intervals
        max_check_attempts      	10                      ; Check each switch 10 times (max)
        check_command           check-host-alive                ; Default command to check if routers are "alive"
        register                        0                       ; DONT REGISTER THIS DEFINITION - ITS NOT A REAL HOST, JUST A TEMPLATE!
        }

# HPCC Service Definition
define service{
        name                            hpcc-nonprod-service         
        active_checks_enabled           1                            
        passive_checks_enabled          1                            
        parallelize_check               1                            
        obsess_over_service             1                            
        check_freshness                 0                            
        notifications_enabled           1                            
        event_handler_enabled           1                            
        flap_detection_enabled          1                            
        process_perf_data               1                            
        retain_status_information       1                            
        retain_nonstatus_information    1                            
        is_volatile                     0                            
        check_period                    hpcc-24x7                    
        max_check_attempts              3                            
        normal_check_interval           10                           
        retry_check_interval            2                            
        contact_groups                  hpcc-admins                       
        notification_options            w,u,c,r                      
        notification_interval           60                          
        notification_period             hpcc-workhours               
         register                        0                           
        }

"""
        config_file.write(definitions_conf)
        config_file.close()


### Creating configuration files

InsertDaliEntry()
InsertSashaEntry()
InsertESPServerEntry()
InsertRoxieEntry()
InsertThorMasterEntry()
EditHostGroups()
hpccTimePeriods()
hpccContacts()
hpccCommands()
hpccProdDefinitions()
hpccNonProdDefinitions()

### Changing file ownership

subprocess.call('chown nagios. /etc/HPCCSystems/nagios/ -R', shell=True)

### Restarting Nagios application

subprocess.call('systemctl restart nagios.service', shell=True)

