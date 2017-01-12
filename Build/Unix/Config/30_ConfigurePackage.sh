#! /bin/bash

###############################################################################
#
# @file Build/Unix/Config/30_ConfigurePackage.sh
#
# Configure package options
#
###############################################################################

PROJECT_DIR_PACKAGE=${PROJECT_DIR_SOURCE}/Package

PROJECT_ISO=${PROJECT_DIR_PACKAGE}/${PROJECT_NAME}
if [ "${BUILD_TARGET}" == "RELEASE" ]; then
  PROJECT_ISO=${PROJECT_ISO}-${PROJECT_VERSION_BASE}
else
  PROJECT_ISO=${PROJECT_ISO}-${PROJECT_VERSION}
fi
PROJECT_ISO=${PROJECT_ISO}.iso

###############################################################################
# Parse script arguments
###############################################################################

COUNTER=1
while [ ${COUNTER} -le $# ]; do
  case ${*:${COUNTER}:1} in
    --no-iso)
      PROJECT_ISO=
      ;;
    --iso)
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_ISO=${*:${COUNTER}:1}
      ;;
    --package-dir)
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_DIR_PACKAGE=${*:${COUNTER}:1}
      ;;
    --iso=*)
      PROJECT_ISO=${*:${COUNTER}:1}
      PROJECT_ISO=${PROJECT_ISO:6}
      ;;
    --package-dir=*)
      PROJECT_DIR_PACKAGE=${*:${COUNTER}:1}
      PROJECT_DIR_PACKAGE=${PROJECT_DIR_PACKAGE:16}
      ;;
    *) ;;
  esac
  let COUNTER+=1
done

###############################################################################
# Prepare package
###############################################################################

if [ -z ${PROJECT_DIR_PACKAGE:+x} ]; then
  PROJECT_DIR_PACKAGE=${PROJECT_DIR_SOURCE}/Package
fi
printAlign Output: ${PROJECT_DIR_PACKAGE/#${WORKSPACE}\//}

if [ ! -z ${PROJECT_ISO:+x} ]; then
  printAlign ISO: ${PROJECT_ISO/#${WORKSPACE}\//}
fi

