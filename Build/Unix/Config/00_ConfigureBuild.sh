#! /bin/bash

###############################################################################
#
# @file Build/Unix/Config/00_ConfigureBuild.sh
#
# Configure build options
#
###############################################################################

BUILD_CONFIG_DIR=
BUILD_CONFIG_FILE=
BUILD_TOOL_CHAIN_DEFAULT=ELFGCC
BUILD_TARGET_DEFAULT=RELEASE
BUILD_ARCH_DEFAULT="IA32 X64"

BUILD_DRYRUN=
BUILD_CLEAN=
BUILD_TOOL_CHAIN=
BUILD_ARCH=
BUILD_TARGET=
BUILD_SOURCE=

PROJECT_BUILD_ARGUMENTS=
PROJECT_DIR_SUPPORT=
PROJECT_DIR_STAGE=
PROJECT_DIR_BUILD=

###############################################################################
# Parse build arguments
###############################################################################

COUNTER=1
while [ ${COUNTER} -le $# ]; do
  echo ${COUNTER} ${*:${COUNTER}:1}
  case "${*:${COUNTER}:1}" in
    "clean")
      BUILD_CLEAN=Yes
      ;;
    "cleanall")
      BUILD_CLEAN=All
      ;;
    "--dryrun")
      BUILD_DRYRUN=Yes
      ;;
    "--lite")
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -DPROJECT_LITE
      PROJECT_BUILD_OPTIONS${PROJECT_BUILD_OPTIONS}\ -DPROJECT_LITE
      ;;
    "-a")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_ARCH=${*:${COUNTER}:1}
      ;;
    "-p")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_SOURCE=${*:${COUNTER}:1}
      ;;
    "-b")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_TARGET=${*:${COUNTER}:1}
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -b\ ${BUILD_TARGET}
      ;;
    "-t")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_TOOL_CHAIN=${*:${COUNTER}:1}
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -t\ ${BUILD_TOOL_CHAIN}
      ;;
    "--arch")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_ARCH=${*:${COUNTER}:1}
      ;;
    "--platform")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_SOURCE=${*:${COUNTER}:1}
      ;;
    "--buildtarget")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_TARGET=${*:${COUNTER}:1}
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -b\ ${BUILD_TARGET}
      ;;
    "--tagname")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_TOOL_CHAIN=${*:${COUNTER}:1}
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -t\ ${BUILD_TOOL_CHAIN}
      ;;
    "--build-dir")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      PROJECT_DIR_BUILD=${*:${COUNTER}:1}
      ;;
    "--conf")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_CONFIG_DIR=${*:${COUNTER}:1}
      ;;
    "--arch=*")
      BUILD_ARCH=${*:${COUNTER}:1}
      BUILD_ARCH=${BUILD_ARCH:7}
      ;;
    "--platform=*")
      BUILD_SOURCE=${*:${COUNTER}:1}
      BUILD_SOURCE=${BUILD_SOURCE:11}
      ;;
    "--buildtarget=*")
      BUILD_TARGET=${*:${COUNTER}:1}
      BUILD_TARGET=${BUILD_TARGET:14}
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -b\ ${BUILD_TARGET}
      ;;
    "--tagname=*")
      BUILD_TOOL_CHAIN=${*:${COUNTER}:1}
      BUILD_TOOL_CHAIN=${BUILD_TOOL_CHAIN:10}
      PROJECT_BUILD_ARGUMENTS=${PROJECT_BUILD_ARGUMENTS}\ -t\ ${BUILD_TOOL_CHAIN}
      ;;
    "--build-dir=*")
      PROJECT_DIR_BUILD=${*:${COUNTER}:1}
      PROJECT_DIR_BUILD=${PROJECT_DIR_BUILD:12}
      ;;
    "--conf=*")
      BUILD_CONFIG_DIR=${*:${COUNTER}:1}
      BUILD_CONFIG_DIR=${BUILD_CONFIG_DIR:7}
      ;;
    *)
      ;;
  esac
  let COUNTER+=1
done

###############################################################################
# Set default build options if options have no value
###############################################################################

if [ -z ${BUILD_TOOL_CHAIN:+x} ]; then
  BUILD_TOOL_CHAIN=${BUILD_TOOL_CHAIN_DEFAULT}
fi
if [ -z ${BUILD_TARGET:+x} ]; then
  BUILD_TARGET=${BUILD_TARGET_DEFAULT}
fi
if [ -z ${BUILD_ARCH:+x} ]; then
  BUILD_ARCH=${BUILD_ARCH_DEFAULT}
fi
if [ -z ${BUILD_SOURCE:+x} ]; then 
  BUILD_SOURCE=${PROJECT_DIR_SOURCE}/Package.dsc
fi

if [ -z ${PROJECT_DIR_SUPPORT:+x} ]; then
  PROJECT_DIR_SUPPORT=${PROJECT_DIR_SOURCE}/Build/Support
fi
if [ -z ${PROJECT_DIR_STAGE:+x} ]; then
  PROJECT_DIR_STAGE=${PROJECT_DIR_SOURCE}/Build/Stage
fi
if [ -z ${PROJECT_DIR_BUILD:+x} ]; then
  PROJECT_DIR_BUILD=${PROJECT_DIR_STAGE}/${BUILD_TARGET}_${BUILD_TOOL_CHAIN}
fi

###############################################################################
# Set the default build options
###############################################################################

if [ -z ${BUILD_CONFIG_DIR:+x} ]; then
  BUILD_CONFIG_DIR=${PROJECT_DIR_SUPPORT}/Conf
fi
PROJECT_BUILD_ARGUMENTS="${PROJECT_BUILD_ARGUMENTS} --conf=${BUILD_CONFIG_DIR}"
if [ -z ${BUILD_CONFIG_FILE:+x} ]; then
  BUILD_CONFIG_FILE=${BUILD_CONFIG_DIR}/target.txt
fi

###############################################################################
# Print the build options
###############################################################################

printAlign Source: ${BUILD_SOURCE/#${WORKSPACE}\//}
printAlign "Build Stage:" ${PROJECT_DIR_BUILD/#${WORKSPACE}\//}
if [ ! -z ${BUILD_CLEAN:+x} ]; then
  case ${BUILD_CLEAN} in
    All)
      printAlign Clean: ${PROJECT_DIR_STAGE/#${WORKSPACE}\//}
      ;;
    *)
      printAlign Clean: ${PROJECT_DIR_BUILD/#${WORKSPACE}\//}
      ;;
  esac
fi

