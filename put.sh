#!/bin/bash

METRIC_HOME=`dirname "$0"`
METRIC_HOME=`cd "$METRIC_HOME"; pwd`

SERVER_URL=http://localhost:8000
COOKIE_PATH=/tmp/django_cookie.txt

curl -s -c ${COOKIE_PATH} ${SERVER_URL}/admin/login/ > /dev/null 2>&1
TOKEN=`grep "csrftoken" ${COOKIE_PATH} | awk '{ print $7 }'`

TEMP_HUMID_DATA=`sudo $METRIC_HOME/temphumid`
echo $TEMP_HUMID_DATA

curl -s -c ${COOKIE_PATH} -b ${COOKIE_PATH} -H "X-CSRFTOKEN:${TOKEN}" -X POST -d "data=${TEMP_HUMID_DATA}" ${SERVER_URL}/collector/put

