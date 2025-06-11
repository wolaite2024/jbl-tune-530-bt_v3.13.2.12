#!/bin/bash

# template file
TEMPLATE_FILE="git_version.template"

TEMPLATE_PATH=$2/$TEMPLATE_FILE
VERSION_PATH=$3
#echo $TEMPLATE_PATH
#echo $VERSION_PATH

# check if git is valid
git --version > /dev/null 2>&1
err=$?
if [ $err -ne 0 ]; then
    echo "no git tool, use default $VERSION_PATH"
    exit 0
fi

git rev-parse --git-dir > /dev/null 2>&1
err=$?
if [ $err -ne 0 ]; then
    echo "not git dir, use default $VERSION_PATH"
    exit 0
fi

# echo "generate version"

# get full describe version
FULL_VERSION=`git describe`
# set version to 0.0.0 if no tag name
if [ ! $FULL_VERSION ]; then
    TAG_HEADER="-v"
    FULL_VERSION=$TAG_HEADER"0.0.0"
fi
# echo $FULL_VERSION

# remove tag header
PROJ_VERSION=${FULL_VERSION##*-v}
# echo $PROJ_VERSION

# split version to five fields
TAG_VERSION=${FULL_VERSION##*-v}
# echo $TAG_VERSION
MAJOR=`echo $TAG_VERSION | awk -F '.' '{print $1}'`
if [ ! $MAJOR ]; then
    MAJOR=0
fi

MINOR=`echo $TAG_VERSION | awk -F '.' '{print $2}'`
if [ ! $MINOR ]; then
    MINOR=0
fi

# App: get revision from SDK tag
REVISION=`echo $TAG_VERSION | awk -F '.' '{print $3}'`
REVISION=${REVISION%%-*}
if [ ! $REVISION ]; then
    REVISION=0
fi

# Patch: using count number as revision
BUILDNUM=`echo $PROJ_VERSION | awk -F '-' '{print $2}'`
if [ ! $BUILDNUM ]; then
    BUILDNUM=0
fi
if [ $BUILDNUM -gt 2048 ]; then
    echo "Build number: " $BUILDNUM
    echo "Build number too large, Need to update revision !!!"
    exit -1
fi
#echo $BUILDNUM

#get branch name
CUSTOMER_NAME=$4
if [ ! $CUSTOMER_NAME ]; then
    BRANCH_NAME=`git symbolic-ref --short -q HEAD`
    CUSTOMER_NAME=`echo $BRANCH_NAME | awk -F '-' '{print $2}'`

    if [ ! $CUSTOMER_NAME ]; then
        CUSTOMER_NAME="unknown"
    fi
fi

# echo $CUSTOMER_NAME
CN_1=${CUSTOMER_NAME:0:1}
CN_2=${CUSTOMER_NAME:1:1}
CN_3=${CUSTOMER_NAME:2:1}
if [ ! $CN_3 ]; then
    CN_3=#
fi
CN_4=${CUSTOMER_NAME:3:1}
if [ ! $CN_4 ]; then
    CN_4=#
fi
CN_5=${CUSTOMER_NAME:4:1}
if [ ! $CN_5 ]; then
    CN_5=#
fi
CN_6=${CUSTOMER_NAME:5:1}
if [ ! $CN_6 ]; then
    CN_6=#
fi
CN_7=${CUSTOMER_NAME:6:1}
if [ ! $CN_7 ]; then
    CN_7=#
fi
CN_8=${CUSTOMER_NAME:7:1}
if [ ! $CN_8 ]; then
    CN_8=#
fi

#get commit ID
GIT_CMTID_RAW=`git log --pretty=oneline -1 | cut -c 1-12`
GIT_CMTID=0x${GIT_CMTID_RAW:0:8}

# echo $MAJOR.$MINOR.$REVISION.$BUILDNUM

# build time
BUILDING_TIME=`env LANG=en_US.UTF-8 date '+%a %b %e %R:%S %Y'`
BUILDING_TIME_STR=`env LANG=en_US.UTF-8 date -d "$BUILDING_TIME" '+%a_%Y_%m_%d_%H_%M_%S'`
#echo $BUILDING_TIME

# get builder name
BUILDER_MAIL=`git config user.email`
if [ ! $BUILDER_MAIL ]
then
    BUILDER_MAIL="server@realsil.com.cn"
fi
BUILDER=`echo ${BUILDER_MAIL%@*}`
BUILDER_STR=${BUILDER//./_}
#echo $BUILDER_STR

# substitute version&time in template
`cat $TEMPLATE_PATH | sed -e "s/MAJOR_T/$MAJOR/g" \
                     -e "s/MINOR_T/$MINOR/g" \
                     -e "s/REVISION_T/$REVISION/g" \
                     -e "s/BUILDNUM_T/$BUILDNUM/g" \
                     -e "s/BUILDTIME_T/$BUILDING_TIME/g"    \
                     -e "s/GITCMTID_T/$GIT_CMTID/g" \
                     -e "s/CUSTOMER_NAME_T/$CUSTOMER_NAME/g" \
                     -e "s/CN_1_T/$CN_1/g" \
                     -e "s/CN_2_T/$CN_2/g" \
                     -e "s/CN_3_T/$CN_3/g" \
                     -e "s/CN_4_T/$CN_4/g" \
                     -e "s/CN_5_T/$CN_5/g" \
                     -e "s/CN_6_T/$CN_6/g" \
                     -e "s/CN_7_T/$CN_7/g" \
                     -e "s/CN_8_T/$CN_8/g" \
                     -e "s/GIT_CMTID_RAW_T/$GIT_CMTID_RAW/g" \
                     -e "s/BUILDING_TIME_STR_T/$BUILDING_TIME_STR/g" \
                     -e "s/BUILDER_T/$BUILDER/g" \
                     -e "s/BUILDER_STR_T/$BUILDER_STR/g" \
                    > $VERSION_PATH`

# show result
echo "Generated Version File :"
cat $VERSION_PATH
