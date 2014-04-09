#ifndef _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
#define _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_

static const char *PCONFIGGEN_PARAM_LIST(" -list ");
static const char *PCONFIGGEN_PARAM_LIST_ALL(" -listall ");
static const char *PCONFIGGEN_PARAM_ENVIRONMENT(" -env ");
static const char *PCONFIGGEN_PARAM_MACHINES(" -machines ");

/*
define host{
        use                     generic-host  ; Name of host template to use
        host_name               server02
        alias                   Server 02
        address                 172.18.100.101
}
*/
static const char *P_NAGIOS_HOST_CONFIG_1("define host\n\
{\n\
    use         generic-host\n\
    host_name   ");
static const char *P_NAGIOS_HOST_CONFIG_2("\n\
    alias       ");
static const char *P_NAGIOS_HOST_CONFIG_3("\n\
    address     ");
static const char *P_NAGIOS_HOST_CONFIG_4("\n\
}\n");


/*
define service {
          use                             generic-service
          host_name                       server02
          service_description             DNS
          check_command                   check_dns!172.18.100.101
}
*/
static const char *P_NAGIOS_SERVICE_CONFIG_1("define service\n\
    {\n\
        use                 generic-service\n\
        host_name   ");
static const char *P_NAGIOS_SERVICE_CONFIG_2("\
        service_description ");
static const char *P_NAGIOS_SERVICE_CONFIG_3("\
        check_command       ");
static const char *P_NAGIOS_SERVICE_CONFIG_4("!");  // NOT SURE IF THIS IS USEFUL
static const char *P_NAGIOS_SERVICE_CONFIG_5("\n\
    }\n");

/*
define service {
        hostgroup_name        mysql-servers
        service_description   MySQL
        check_command         check_mysql_cmdlinecred!nagios!secret!$HOSTADDRESS
        use                   generic-service
        notification_interval 0 ; set > 0 if you want to be renotified
}
*/
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_1("define service\n\
    {\n\
        hostgroup_name      ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_2("\
        service_description ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_3("\
        check_command       ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_4("\
        use                 generic-service\n\
        notification_interval 0\n\
    }\n");


/*
define hostgroup {
        hostgroup_name  mysql-servers
                alias           MySQL servers
                members         localhost, server02
        }
*/

#endif // _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
