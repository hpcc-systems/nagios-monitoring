#!/bin/bash

# Prerequistes Debian (Ubuntu 12.04 LTS)
# interactive script
#apt-get install apache2 libapache2-mod-php5 build-essential libgd2-xpm-dev nagios-plugins nagios3 nagios-nrpe-plugin;   \

# Add Nagios user 
#/usr/sbin/useradd -s /bin/bash nagios; \

# !!! Change Default Password !!!
#echo -e "nagios\nnagios" | `passwd nagios` ; \

/usr/sbin/groupadd nagcmd; \
/usr/sbin/usermod -a -G nagcmd nagios; \
/usr/sbin/usermod -a -G nagcmd www-data; \








 
