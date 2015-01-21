#ifndef _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
#define _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_

#include "jstring.hpp"
#include "jhash.hpp"

static const int   DEFAULT_TIMEOUT(5*1000);
static const int   DALI_CHECK_TIMEOUT(DEFAULT_TIMEOUT);
static const int   SASHA_CHECK_TIMEOUT(DEFAULT_TIMEOUT);

static const char *P_CHECK_NRPE("check_nrpe!");
static const char *P_CHECK_NRPE_1_ARG("check_nrpe_1_arg!");
static const char *P_CHECK_NRPE_2_ARG("check_nrpe_2_arg!");

static const char *P_CONFIGGEN_PARAM_LIST(" -list ");
static const char *P_CONFIGGEN_PARAM_LIST_ALL(" -listall ");
static const char *P_CONFIGGEN_PARAM_ENVIRONMENT(" -env ");
static const char *P_CONFIGGEN_PARAM_MACHINES(" -machines ");
static const char *P_CONFIGGEN_PARAM_LIST_ESP_SERVICES(" -listespservices ");
static const char *P_CHECK_ESP_SERVICE("check_esp_service_");
static const char *P_CHECK_SSH_SERVICE("check_ssh_hpcc_machine");
static const char *P_CHECK_DALI("check_dali");
static const char *P_CHECK_SASHA("check_sasha");
static const char *P_CHECK_ROXIE("check_roxie");
static const char *P_CHECK_DAFILESRV("check_dafilesrv");
static const char *P_HTTP("http");
static const char *P_HTTPS("https");
static const char *P_BY_TYPE(" -t ");

struct NodeName
{
    StringBuffer strHostName;
    StringBuffer strHostAlias;
};

typedef MapStringTo<NodeName> MapIPtoNode;

/*
define host {
        host_name               server02
        alias                   Server 02
        address                 172.18.100.101


        max_check_attempts      10
        check_period            24x7
        contacts                contacts
        contact_groups          contact_groups
        notification_interval   10
        notification_period     24x7
}
*/
static const char *P_NAGIOS_HOST_CONFIG_1("define host{\n\
    host_name           ");
static const char *P_NAGIOS_HOST_CONFIG_ALIAS("\n\
    alias               ");
static const char *P_NAGIOS_HOST_CONFIG_ADDRESS("\n\
    address             ");
static const char *P_NAGIOS_HOST_CONFIG_END("\n\
}\n");
static const char *P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS("\n\
    max_check_attempts  ");
static const char *P_NAGIOS_HOST_CONFIG_CHECK_PERIOD("\n\
    check_period            ");
static const char *P_NAGIOS_HOST_CONFIG_CONTACTS("\n\
    contacts                ");
static const char *P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS("\n\
    contact_groups          ");
static const char *P_NAGIOS_HOST_CONFIG_NOTIFICATION_INTERVAL("\n\
    notification_interval   ");
static const char *P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD("\n\
    notification_period      ");

/*
define service {
          use                             generic-service
          host_name                       server02
          service_description             DNS
          check_command                   check_dns!172.18.100.101
}
*/
static const char *P_NAGIOS_SERVICE_CONFIG_1("define service {\n\
    use                  generic-service\n\
    host_name            ");
static const char *P_NAGIOS_SERVICE_DESCRIPTION("\n\
    service_description  ");
static const char *P_NAGIOS_SERVICE_CHECK_COMMAND("\n\
    check_command        ");
static const char *P_NAGIOS_SEPERATOR("!");
static const char *P_NAGIOS_SERVICE_END_BRACKET("\n\
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
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_1("define service{\n\
    hostgroup_name       ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_2("\n\
    service_description  ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_3("\n\
    check_command        ");
static const char *P_NAGIOS_SERVICE_DEFINITION_CONFIG_4("\n\
    use                  generic-service\n\
    notification_interval  0\n\
}\n");


/*
define hostgroup {
        hostgroup_name  mysql-servers
                alias           MySQL servers
                members         localhost, server02
        }
*/

static const char *P_NAGIOS_HOSTS_GROUP_CONFIG_1("define hostgroup{\n\
    hostgroup_name      ");
static const char *P_NAGIOS_HOSTS_GROUP_ALIAS("\n\
    alias               ");
static const char *P_NAGIOS_HOSTS_GROUP_MEMBERS("\n\
    members             ");
static const char *P_NAGIOS_HOSTS_GROUP_END_BRACKET("\n\
}\n");

#endif // _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
