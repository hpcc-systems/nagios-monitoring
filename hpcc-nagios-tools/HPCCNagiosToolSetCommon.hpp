#ifndef _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
#define _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_

#include "jstring.hpp"
#include "jhash.hpp"

static constexpr int   DEFAULT_TIMEOUT(5*1000);
static constexpr int   DALI_CHECK_TIMEOUT(DEFAULT_TIMEOUT);
static constexpr int   SASHA_CHECK_TIMEOUT(DEFAULT_TIMEOUT);

static constexpr const char P_CHECK_NRPE[] = "check_nrpe!";
static constexpr const char P_CHECK_NRPE_1_ARG[] = "check_nrpe_1_arg!";
static constexpr const char P_CHECK_NRPE_2_ARG[] = "check_nrpe_2_arg!";

static constexpr const char P_CONFIGGEN_PARAM_LIST[] = " -list ";
static constexpr const char P_CONFIGGEN_PARAM_LIST_ALL[] = " -listall ";
static constexpr const char P_CONFIGGEN_PARAM_ENVIRONMENT[] = " -env ";
static constexpr const char P_CONFIGGEN_PARAM_MACHINES[] = (" -machines ");
static constexpr const char P_CONFIGGEN_PARAM_LIST_ESP_SERVICES[] = " -listespservices ";
static constexpr const char P_CHECK_ESP_SERVICE[] = "check_esp_service_";
static constexpr const char P_CHECK_SSH_SERVICE[] = "check_ssh_hpcc_machine";
static constexpr const char P_CHECK_DALI[] = "check_dali";
static constexpr const char P_CHECK_SASHA[] = "check_sasha";
static constexpr const char P_CHECK_ROXIE[] = "check_roxie";
static constexpr const char P_SSH[] = "ssh";
static constexpr const char P_CHECK_DAFILESRV[] = "check_dafilesrv";
static constexpr const char P_CHECK_THORMASTER[] = "check_thormaster";
static constexpr const char P_HTTP[] = "http";
static constexpr const char P_HTTPS[] = "https";
static constexpr const char P_BY_TYPE[] = " -t ";

static constexpr const char P_NAGIOS_HOSTADDRESS[] = "'$HOSTADDRESS$'";

static constexpr const char P_NAGIOS_HOSTSTATE[] = "'$HOSTSTATE$'";
static constexpr const char P_NAGIOS_HOSTNOTES[] = "'$HOSTNOTES$";
static constexpr const char P_NAGIOS_LASTHOSTCHECK[] = "'$LASTHOSTCHECK$'";

static const char P_NAGIOS_SERVICESTATE[] = "'$SERVICESTATE$'";
static const char P_NAGIOS_LASTSERVICECHECK[] = "'$LASTSERVICECHECK$'";
static const char P_NAGIOS_SERVICENOTES[] = "'$SERVICENOTES$'";
static const char P_NAGIOS_DISPLAYNAME[] = "'$DISPLAYNAME'";


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
        display_name            host
        check_command           check-host-alive!!!!!!!!
        max_check_attempts      10
        check_period            24x7
        contacts                eclwatch
        contact_groups          eclwatch_group
        notification_interval   10
        notification_period     24x7
        notifications_enabled   1
}
*/
static constexpr const char P_NAGIOS_HOST_CONFIG_1[] = "define host{\n\
    host_name                       ";
static constexpr const char P_NAGIOS_HOST_CONFIG_ALIAS[] = "\n\
    alias                           ";
static constexpr const char P_NAGIOS_HOST_CONFIG_ADDRESS[] = "\n\
    address                         ";
static const char P_NAGIOS_HOST_DISPLAY_NAME[] = "\n\
    display_name                    ";
static constexpr const char P_NAGIOS_HOST_CONFIG_END[] = "\n\
}\n";
static constexpr const char P_NAGIOS_HOST_CHECK_COMMAND[] = "\n\
    check_command                   ";
static constexpr const char P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS[] = "\n\
    max_check_attempts              ";
static constexpr const char P_NAGIOS_HOST_CONFIG_CHECK_PERIOD[] = "\n\
    check_period                    ";
static constexpr const char P_NAGIOS_HOST_CONFIG_CONTACTS[] = "\n\
    contacts                        ";
static constexpr const char P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS[] = "\n\
    contact_groups                  ";
static constexpr const char P_NAGIOS_HOST_CONFIG_NOTIFICATION_INTERVAL[] = "\n\
    notification_interval           ";
static const char P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD[] = "\n\
    notification_period             ";
static constexpr const char P_NAGIOS_HOST_CONFIG_NOTIFICATIONS_ENABLED[] = "\n\
    notifications_enabled           ";

/*
define service {
#         use                             generic-service

            active_checks_enabled           1
            passive_checks_enabled          1
            parallelize_check               1
            obsess_over_service             1
            check_freshness                 0
            event_handler_enabled           1
            flap_detection_enabled          1
            failure_prediction_enabled      1
            process_perf_data               1
            retain_status_information       1
            retain_nonstatus_information    1
            notification_interval           0
            is_volatile                     0
            normal_check_interval           5
            retry_check_interval            1


          host_name                       server02
          service_description             DNS
          display_name                    EspProcess
          check_command                   check_dns!172.18.100.101
          max_check_attempts              10
          check_interval                  5
          retry_interval                  3
          check_period                    24x7
          notification_period             24x7
          notifications_enabled           1
          notification_options            w,u,c,r,f
#          contacts                        eclwatch
          contact_groups                  eclwatch_group
          notes                           esp:8015
}
*/
static constexpr const char P_NAGIOS_SERVICE_CONFIG_[] = "define service {\n\
    use                             generic-service\n\
    host_name                       ";
static constexpr const char P_NAGIOS_SERVICE_CONFIG_2[] = "define service {\n\
    host_name                       ";

static constexpr const char P_NAGIOS_SERVICE_ACTIVE_CHECKS_ENABLED[] = "\n\
    active_checks_enabled           ";
static constexpr const char P_NAGIOS_SERVICE_PASSIVE_CHECKS_ENABLED[] = "\n\
    passive_checks_enabled          ";
static constexpr const char P_NAGIOS_SERVICE_PARALLELIZE_CHECK[] = "\n\
    parallelize_check               ";
static constexpr const char P_NAGIOS_SERVICE_OBSESS_OVER_SERVICE[] = "\n\
    obsess_over_service             ";
static constexpr const char P_NAGIOS_SERVICE_CHECK_FRESHNESS[] = "\n\
    check_freshness                 ";
static constexpr const char P_NAGIOS_SERVICE_EVENT_HANDLER_ENABLED[] = "\n\
    event_handler_enabled           ";
static constexpr const char P_NAGIOS_SERVICE_FLAP_DETECTION_ENABLED[] = "\n\
    flap_detection_enabled          ";
static constexpr const char P_NAGIOS_SERVICE_FAILURE_PREDICTION_ENABLED[] = "\n\
    failure_prediction_enabled      ";
static constexpr const char P_NAGIOS_SERVICE_PROCESS_PERF_DATA[] = "\n\
    process_perf_data               ";
static constexpr const char P_NAGIOS_SERVICE_RETAIN_STATUS_INFORMATION[] = "\n\
    retain_status_information       ";
static constexpr const char P_NAGIOS_SERVICE_RETAIN_NONSTATUS_INFORMATION[] = "\n\
    retain_nonstatus_information    ";
static constexpr const char P_NAGIOS_SERVICE_NOTIFICATION_INTERVAL[] = "\n\
    notification_interval           ";
static constexpr const char P_NAGIOS_SERVICE_IS_VOLATILE[] = "\n\
    is_volatile                     ";
static constexpr const char P_NAGIOS_SERVICE_NORMAL_CHECK_INTERVAL[] = "\n\
    normal_check_interval           ";
static constexpr const char P_NAGIOS_SERVICE_RETRY_CHECK_INTERVAL[] = "\n\
    retry_check_interval            ";
static constexpr const char P_NAGIOS_SERVICE_DESCRIPTION[] = "\n\
    service_description             ";
static constexpr const char P_NAGIOS_SERVICE_DISPLAY_NAME[] = "\n\
    display_name                    ";
static constexpr const char P_NAGIOS_SERVICE_CHECK_COMMAND[] = "\n\
    check_command                   ";
static constexpr const char P_NAGIOS_SEPERATOR[] = "!";
static constexpr const char *P_NAGIOS_SERVICE_MAX_CHECK_ATTEMPTS =  P_NAGIOS_HOST_CONFIG_MAX_CHECK_ATTEMPTS;
static constexpr const char P_NAGIOS_SERVICE_RETRY_INTERVAL[] = "\n\
    retry_interval                  ";
static constexpr const char P_NAGIOS_SERVICE_CHECK_INTERVAL[] = "\n\
    check_interval                  ";
static constexpr const char *P_NAGIOS_SERVICE_CHECK_PERIOD = P_NAGIOS_HOST_CONFIG_CHECK_PERIOD;
static constexpr const char *P_NAGIOS_SERVICE_NOTIFICATION_PERIOD = P_NAGIOS_HOST_CONFIG_NOTIFICATION_PERIOD;
static constexpr const char P_NAGIOS_SERVICE_NOTIFICATION_OPTIONS[] = ("\n\
    notification_optioins           ");
static constexpr const char *P_NAGIOS_SERVICE_NOTIFICATIONS_ENABLED = P_NAGIOS_HOST_CONFIG_NOTIFICATIONS_ENABLED;
static constexpr const char *P_NAGIOS_SERVICE_CONTACTS = P_NAGIOS_HOST_CONFIG_CONTACTS;
static constexpr const char *P_NAGIOS_SERVICE_CONTACT_GROUPS = P_NAGIOS_HOST_CONFIG_CONTACT_GROUPS;
static constexpr const char P_NAGIOS_SERVICE_NOTES[] = ("\n\
    notes                           ");
static constexpr const char P_NAGIOS_SERVICE_END_BRACKET[] = ("\n\
}\n");

/*
define hostgroup {
        hostgroup_name  mysql-servers
                alias           MySQL servers
                members         localhost, server02
        }
*/

static constexpr const char *P_NAGIOS_HOSTS_GROUP_CONFIG_1("define hostgroup{\n\
    hostgroup_name      ");
static constexpr const char *P_NAGIOS_HOSTS_GROUP_ALIAS("\n\
    alias               ");
static constexpr const char *P_NAGIOS_HOSTS_GROUP_MEMBERS("\n\
    members             ");
static constexpr const char *P_NAGIOS_HOSTS_GROUP_END_BRACKET("\n\
}\n");

/*
# 'send_host_status' command definition
define command{
        command_name    send_host_status
        command_line    /usr/lib/nagios/plugins/send_status localhost:8010 -h $HOSTADDRESS$ $HOSTSTATE$ $HOSTNOTES$ 2> /dev/null
        }

# 'send_service_status' command definition
define command{
        command_name    send_service_status
        command_line    /usr/lib/nagios/plugins/send_status localhost:8010 -s $HOSTADDRESS$ $SERVICESTATE$ $SERVICENOTES$ 2> /dev/null
        }

*/

static constexpr const char P_NAGIOS_COMMAND_CONFIG_1[] = ("\n\
define command{        ");
static constexpr const char P_NAGIOS_COMMAND_NAME[] = ("\n\
        command_name    ");
static constexpr const char P_NAGIOS_COMMAND_LINE[] = ("\n\
        command_line    ");
static constexpr const char *P_NAGIOS_COMMAND_END_BRACKET = P_NAGIOS_SERVICE_END_BRACKET;

#endif // _HPCC_NAGIOS_TOOL_SET_COMMON_HPP_
