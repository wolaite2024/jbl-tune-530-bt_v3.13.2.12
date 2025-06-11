#!/bin/bash

CURDIR="`dirname $0`"
# Paths
VERSION_TOOL_PATH=$CURDIR"/version_generation/GitGenerateVersion.sh"
TAG_HEADER="bb2-sdk-"
TEMPLATE_FILE_PATH=$CURDIR"/version_generation"

if [ $1 ]
then
    TARGET_VERSION_FILE_PATH=$1
else
    #TARGET_VERSION_FILE_PATH="../../../inc/platform/version.h"
    TARGET_VERSION_FILE_PATH="version.h"
fi

# Customer Name
CUSTOMER_NAME="sdk-v1"

# call version generation tool to generate the version file
$VERSION_TOOL_PATH $TAG_HEADER $TEMPLATE_FILE_PATH $TARGET_VERSION_FILE_PATH $CUSTOMER_NAME
