#! /bin/bash

###############################################################################
#
# @file Build/Unix/BuildProject.sh
#
# Project package build script
#
###############################################################################

###############################################################################
# Setup the build script directories
###############################################################################

BUILD_DIR_SUPPORT=${PWD}/Support
BUILD_DIR_CONFIG=${PWD}/Config
BUILD_DIR_CLEAN=${PWD}/Clean
BUILD_DIR_POST=${PWD}/Post

###############################################################################
# Setup the build tools
###############################################################################

TOOL_BUILD="${BUILD_DIR_SUPPORT}/InternalBuild.sh"
TOOL_MKDIR="mkdir -p"
TOOL_DATETIME=date
TOOL_WHERE=which
TOOL_TEE=tee

###############################################################################
# Check if run from console
###############################################################################

case "$-" in
  *i*) BUILD_RUNFROMCONSOLE=Yes ;;
  *) BUILD_RUNFROMCONSOLE=No ;;
esac

###############################################################################
# Stripe output
###############################################################################

function printStripe () {
  printf "%s\r\n" "--------------------------------------------------------------------------------"
  printf "$*\r\n"
  printf "%s\r\n" "--------------------------------------------------------------------------------"
  printf "\r\n"
}

###############################################################################
# Exit script
###############################################################################

function exitScript () {
  BUILD_FINISH_DATE=$(${TOOL_DATETIME} +"%a %m/%d/%Y")
  BUILD_FINISH_TIME=$(${TOOL_DATETIME} +"%T.%N")
  BUILD_FINISH_TIME=${BUILD_FINISH_TIME::-7}
  let BUILD_FINISH="(1${BUILD_FINISH_TIME:0:2}-100)*360000+(1${BUILD_FINISH_TIME:3:2}-100)*6000+(1${BUILD_FINISH_TIME:6:2}-100)*100+(1${BUILD_FINISH_TIME:9:2}-100)"
  let BUILD_DURATION="${BUILD_FINISH}-${BUILD_START}"
  let BUILD_DURATION_HOURS="${BUILD_DURATION}/360000"
  if [ ${BUILD_DURATION_HOURS} -lt 10 ]; then
    BUILD_DURATION_HOURS=0${BUILD_DURATION_HOURS}
  fi
  let BUILD_DURATION="${BUILD_DURATION}%360000"
  let BUILD_DURATION_MINS="${BUILD_DURATION}/6000"
  if [ ${BUILD_DURATION_MINS} -lt 10 ]; then
    BUILD_DURATION_MINS=0${BUILD_DURATION_MINS}
  fi
  let BUILD_DURATION="${BUILD_DURATION}%6000"
  let BUILD_DURATION_SECS="${BUILD_DURATION}/100"
  if [ ${BUILD_DURATION_SECS} -lt 10 ]; then
    BUILD_DURATION_SECS=0${BUILD_DURATION_SECS}
  fi
  let BUILD_DURATION="${BUILD_DURATION}%100"
  BUILD_DURATION=${BUILD_DURATION_HOURS}:${BUILD_DURATION_MINS}:${BUILD_DURATION_SECS}.${BUILD_DURATION}
  if [ ${1} -ne 0 ]; then
    printStripe Build failed at ${BUILD_FINISH_TIME} on ${BUILD_FINISH_DATE} after ${BUILD_DURATION}
  else
    printStripe Build finished at ${BUILD_FINISH_TIME} on ${BUILD_FINISH_DATE} after ${BUILD_DURATION}
  fi
  popd >/dev/nul
  if [ "${BUILD_RUNFROMCONSOLE}" == "Yes" ]; then
    read -n1 -r -p "Press any key to continue..."
  fi
  set -e
  exit $1
}

###############################################################################
# Search for the EDK2 workspace which should be parent of this script
###############################################################################

function searchForEDK2Workspace () {
  if [ "$(dirname "${PWD}")" == "${PWD}" ]; then
    echo No EDK2 workspace found. Did you place the sources inside of EDK2 workspace?
    exitScript 1
  fi
  if [ -e "${PWD}/edksetup.sh" ]; then
    source "${PWD}/edksetup.sh" >/dev/nul 2>&1
    return
  fi
  cd ..
  searchForEDK2Workspace
}

pushd . >/dev/nul 2>&1
TEMP_PACKAGE=${PWD}
if [ -z ${WORKSPACE:+x} ]; then
  searchForEDK2Workspace
fi

###############################################################################
# Set the build date and time
###############################################################################

BUILD_START_DATE=$(${TOOL_DATETIME} +"%a %m/%d/%Y")
BUILD_START_TIME=$(${TOOL_DATETIME} +"%T.%N")
BUILD_START_TIME=${BUILD_START_TIME::-7}
let BUILD_START="(1${BUILD_START_TIME:0:2}-100)*360000+(1${BUILD_START_TIME:3:2}-100)*6000+(1${BUILD_START_TIME:6:2}-100)*100+(1${BUILD_START_TIME:9:2}-100)"

###############################################################################
# Get the package directory from the script path
###############################################################################

TEMP_PACKAGE=${TEMP_PACKAGE/${WORKSPACE}\//}

###############################################################################
# Set the default package and source directory
###############################################################################

PROJECT_PACKAGE=${TEMP_PACKAGE/\/*/}
PROJECT_DIR_SOURCE=${WORKSPACE}/${PROJECT_PACKAGE}
PACKAGES_PATH=${PROJECT_DIR_SOURCE}

###############################################################################
# Set the default log directory
###############################################################################

PROJECT_DIR_LOG=${PROJECT_DIR_SOURCE}/Build/Logs

###############################################################################
# Set the default build log file
###############################################################################

BUILD_LOG=${PROJECT_DIR_LOG}/${PROJECT_PACKAGE}-${BUILD_START_DATE:4:2}-${BUILD_START_DATE:7:2}-${BUILD_START_DATE:10:4}-${BUILD_START_TIME//:/.}.log

###############################################################################
# Parse script arguments for the log file, version and help
###############################################################################

COUNTER=1
while [ ${COUNTER} -le $# ]; do
  case ${*:${COUNTER}:1} in
    --no-log)
      BUILD_LOG=
      ;;
    cleanall)
      BUILD_LOG=
      ;;
    --build-log)
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break;
      fi
      BUILD_LOG=${*:${COUNTER}:1}
      ;;
    -?)
      build --help
      exit
      ;;
    -h)
      build --help
      exit
      ;;
    --help)
      build --help
      exit
      ;;
    --version)
      build --version
      exit
      ;;
    *) ;;
  esac
  let COUNTER+=1
done

###############################################################################
# Start build
###############################################################################

cd ${PROJECT_DIR_SOURCE}
if [ ! -z ${BUILD_LOG:+x} ]; then
  if [ ! -d "${PROJECT_DIR_LOG}" ]; then
    echo Creating "${PROJECT_DIR_LOG}"
    ${TOOL_MKDIR} "${PROJECT_DIR_LOG}" 2>&1
  fi
  RESULT=$?
  if [ ${RESULT} -ne 0 ]; then
    exitScript ${RESULT}
  fi
  (source ${TOOL_BUILD} $* 2>&1) | ${TOOL_TEE} "${BUILD_LOG}"
else
  source ${TOOL_BUILD} $* 2>&1
fi

