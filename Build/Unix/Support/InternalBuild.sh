#! /bin/bash

###############################################################################
#
# @file Build/Unix/Support/InternalBuild.sh
#
# Project package internal build script
#
###############################################################################

PROJECT_BUILD_OPTIONS=
PROJECT_BUILD_ARGUMENTS=

###############################################################################
# Align output
###############################################################################

function printAlign () {
  printf "% -16s" "${1}"
  printf "${*:2}\r\n"
}

###############################################################################
# Print start time and build information
###############################################################################

printStripe Build started at ${BUILD_START_TIME} on ${BUILD_START_DATE}
printAlign Parameters: $*
printAlign Workspace: ${WORKSPACE}
printAlign Package: ${PROJECT_PACKAGE}
if [ ! -z ${BUILD_LOG:+x} ]; then
  printAlign BuildLog: ${BUILD_LOG/#${WORKSPACE}\//}
fi
printf "\r\n"

###############################################################################
# Execute the build configuration scripts
###############################################################################

printStripe Executing build configuration scripts
for BUILD_SCRIPT in ${BUILD_DIR_CONFIG}/*.sh; do
  printAlign Executing: ${BUILD_SCRIPT/${BUILD_DIR_CONFIG}\//}
  source "$BUILD_SCRIPT" $*
  RESULT=$?
  if [ ${RESULT} -ne 0 ]; then
    exitScript ${RESULT}
  fi
  printf "\r\n"
done

###############################################################################
# Clean directory
###############################################################################

function cleanDirectory () {
  printAlign Cleaned: ${1}
}

###############################################################################
# Check if cleaning
###############################################################################

if [ ! -z ${BUILD_CLEAN:+x} ]; then
  printStripe Cleaning package
  if [ ! -d "${PROJECT_DIR_PACKAGE}" ]; then
    printAlign Cleaned: ${PROJECT_DIR_PACKAGE}
  else
    cleanDirectory "${PROJECT_DIR_PACKAGE}"
  fi
  RESULT=$?
  if [ ${RESULT} -ne 0 ]; then
    exitScript ${RESULT}
  fi
  if [ "${BUILD_CLEAN}" == "All" ]; then
    if [ ! -d "${PROJECT_DIR_STAGE}" ]; then
      printAlign Cleaned: ${PROJECT_DIR_STAGE/#${WORKSPACE}\//}
    else
      cleanDirectory  "${PROJECT_DIR_STAGE}"
    fi
    if [ ! -d "${PROJECT_DIR_LOG}" ]; then
      printAlign Cleaned: ${PROJECT_DIR_LOG/#${WORKSPACE}\//}
    else
      cleanDirectory  "${PROJECT_DIR_LOG}"
    fi
  else
    if [ ! -d "${PROJECT_DIR_BUILD}" ]; then
      printAlign Cleaned: ${PROJECT_DIR_BUILD/#${WORKSPACE}\//}
    else
      cleanDirectory  "${PROJECT_DIR_BUILD}"
    fi
  fi
  printf "\r\n"
  exitScript $?

fi

###############################################################################
# Build for architecture
###############################################################################

function buildArchitecture () {
  printStripe Building package for ${1}
  printAlign "Stage(${1}):" ${PROJECT_DIR_BUILD/#${WORKSPACE}\//}/${1}
  printf "\r\n"
  build --arch=${1} ${BUILD_PROJECT_ARGUMENTS} ${PROJECT_BUILD_ARGUMENTS} 2>&1
  RESULT=$?
  if [ ${RESULT} -ne 0 ]; then
    exitScript ${RESULT}
  fi
  printf "\r\n"
}

###############################################################################
# Build package
###############################################################################

if [ ! -z ${BUILD_DRYRUN:+x} ]; then
  printStripe DRY RUN: Skipping building package
else
  PROJECT_DIR_STAGE_RELATIVE=${PROJECT_DIR_STAGE/#${WORKSPACE}\//}
  PROJECT_BUILD_OPTIONS="${PROJECT_BUILD_OPTIONS} /I\"${PROJECT_DIR_BUILD}/Include\""
  BUILD_PROJECT_ARGUMENTS="--platform="${BUILD_SOURCE}" -D \"PROJECT_DIR_STAGE=${PROJECT_DIR_STAGE_RELATIVE}\""
  BUILD_PROJECT_ARGUMENTS="${BUILD_PROJECT_ARGUMENTS} -D \"PROJECT_NAME=${PROJECT_NAME}\" -D \"PROJECT_PACKAGE=${PROJECT_PACKAGE}\""
  printStripe Building package
  printAlign Architectures: ${BUILD_ARCH}
  if [ ! -z ${PROJECT_BUILD_ARGUMENTS:+x} ]; then
    printAlign Arguments: ${PROJECT_BUILD_ARGUMENTS}
  fi
  printf "\r\n"

  for i in ${BUILD_ARCH}; do
    buildArchitecture ${i}
  done
fi

###############################################################################
# Execute the post build scripts
###############################################################################

printStripe Executing post build scripts
for BUILD_SCRIPT in ${BUILD_DIR_POST}/*.sh; do
  printAlign Executing: ${BUILD_SCRIPT/#${BUILD_DIR_POST}\//}
  source "${BUILD_SCRIPT}" $*
  RESULT=$?
  if [ ${RESULT} -ne 0 ]; then
    exitScript ${RESULT}
  fi
  printf "\r\n"
done

###############################################################################
#
###############################################################################

###############################################################################
#
###############################################################################

###############################################################################
#
###############################################################################

exitScript 0
