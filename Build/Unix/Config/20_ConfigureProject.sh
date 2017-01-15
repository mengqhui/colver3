#! /bin/bash

###############################################################################
#
# @file Build/Unix/Config/20_ConfigureProject.sh
#
# Configure project options
#
###############################################################################

PROJECT_NAME=

PROJECT_VERSION_FILE=${PROJECT_DIR_SUPPORT}/Version
PROJECT_VERSION_MAJOR_DEFAULT=3
PROJECT_VERSION_MINOR_DEFAULT=0
PROJECT_VERSION_UPDATE_DEFAULT=0
PROJECT_VERSION_TAG_DEFAULT=

PROJECT_FIRMWARE_MAJOR=2
PROJECT_FIRMWARE_MINOR=6
PROJECT_FIRMWARE_UPDATE=0

PROJECT_USER_VERSION=

###############################################################################
# Parse script arguments
###############################################################################

COUNTER=1
while [ ${COUNTER} -le $# ]; do
  case "${*:${COUNTER}:1}" in
    "--project-name")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_NAME=${*:${COUNTER}:1}
      ;;
    "--project-version")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_USER_VERSION=${*:${COUNTER}:1}
      ;;
    "--project-version-file")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_VERSION_FILE=${*:${COUNTER}:1}
      ;;
    "--project-major")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_VERSION_MAJOR=${*:${COUNTER}:1}
      ;;
    "--project-minor")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_VERSION_MINOR=${*:${COUNTER}:1}
      ;;
    "--project-update")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_VERSION_UPDATE=${*:${COUNTER}:1}
      ;;
    "--project-tag")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_VERSION_TAG=${*:${COUNTER}:1}
      ;;
    "--firmware-major")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_FIRMWARE_MAJOR=${*:${COUNTER}:1}
      ;;
    "--firmware-minor")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_FIRMWARE_MINOR=${*:${COUNTER}:1}
      ;;
    "--firmware-update")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_FIRMWARE_UPDATE=${*:${COUNTER}:1}
      ;;
    "--project-name=*")
      PROJECT_NAME=${*:${COUNTER}:1}
      PROJECT_NAME=${PROJECT_NAME:15}
      ;;
    "--project-version=*")
      PROJECT_USER_VERSION=${*:${COUNTER}:1}
      PROJECT_USER_VERSION=${PROJECT_USER_VERSION:18}
      ;;
    "--project-version-file=*")
      PROJECT_VERSION_FILE=${*:${COUNTER}:1}
      PROJECT_VERSION_FILE=${PROJECT_VERSION_FILE:23}
      ;;
    "--project-major=*")
      PROJECT_VERSION_MAJOR=${*:${COUNTER}:1}
      PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR:16}
      ;;
    "--project-minor=*")
      PROJECT_VERSION_MINOR=${*:${COUNTER}:1}
      PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR:16}
      ;;
    "--project-update=*")
      PROJECT_VERSION_UPDATE=${*:${COUNTER}:1}
      PROJECT_VERSION_UPDATE=${PROJECT_VERSION_UPDATE:17}
      ;;
    "--project-tag=*")
      PROJECT_VERSION_TAG=${*:${COUNTER}:1}
      PROJECT_VERSION_TAG=${PROJECT_VERSION_TAG:14}
      ;;
    "--firmware-major=*")
      PROJECT_FIRMWARE_MAJOR=${*:${COUNTER}:1}
      PROJECT_FIRMWARE_MAJOR=${PROJECT_FIRMWARE_MAJOR:17}
      ;;
    "--firmware-minor=*")
      PROJECT_FIRMWARE_MINOR=${*:${COUNTER}:1}
      PROJECT_FIRMWARE_MINOR=${PROJECT_FIRMWARE_MINOR:17}
      ;;
    "--firmware-update=*")
      PROJECT_FIRMWARE_UPDATE=${*:${COUNTER}:1}
      PROJECT_FIRMWARE_UPDATE=${PROJECT_FIRMWARE_UPDATE:18}
      ;;
    *) ;;
  esac
  let COUNTER+=1
done

###############################################################################
# Try to read the version from the version file
###############################################################################

if [ -z ${PROJECT_USER_VERSION:+x} ]; then
  PROJECT_USER_VERSION=$(cat "${PROJECT_VERSION_FILE}")
fi

###############################################################################
# Try to parse the version from user requested version
###############################################################################

if [ ! -z ${PROJECT_USER_VERSION:+x} ]; then

  if [ -z ${PROJECT_VERSION_MAJOR:+x} ]; then
    PROJECT_VERSION_MAJOR=${PROJECT_USER_VERSION/%.*/}
  fi
  if [ -z ${PROJECT_VERSION_MINOR:+x} ]; then
    if [ -z ${PROJECT_VERSION_MAJOR:+x} ]; then
      PROJECT_VERSION_MINOR=${PROJECT_USER_VERSION/#${PROJECT_VERSION_MAJOR}/}
      PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR/#./}
      PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR/%-*/}
      PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR/%.*/}
    fi
  fi
  if [ -z ${PROJECT_VERSION_UPDATE:+x} ]; then
    if [ -z ${PROJECT_VERSION_MINOR:+x} ]; then
      PROJECT_VERSION_UPDATE=${PROJECT_USER_VERSION/#${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}/}
      PROJECT_VERSION_UPDATE=${PROJECT_VERSION_UPDATE/#./}
      PROJECT_VERSION_UPDATE=${PROJECT_VERSION_UPDATE/%-*/}
    fi
  fi
  if [ -z ${PROJECT_VERSION_TAG:+x} ]; then
    if [ -z ${PROJECT_VERSION_UPDATE:+x} ]; then
      PROJECT_VERSION_TAG=${PROJECT_USER_VERSION/#${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_UPDATE}/}
    else
      PROJECT_VERSION_TAG=${PROJECT_USER_VERSION/#${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}/}
    fi
    PROJECT_VERSION_TAG=${PROJECT_VERSION_TAG/%-/}
  fi

fi

###############################################################################
# Get the project name
###############################################################################

if [ -z ${PROJECT_NAME:+x} ]; then
  PROJECT_NAME=${PROJECT_PACKAGE/%Pkg/}
fi
PROJECT_SAFE_NAME=${PROJECT_NAME^^}
printAlign Project: ${PROJECT_NAME} "(${PROJECT_SAFE_NAME})"

###############################################################################
# Get the build date and time
###############################################################################

PROJECT_DATETIME=$(date +"%Y/%m/%d %T")
printAlign Date/Time: ${PROJECT_DATETIME}

###############################################################################
# Determine the project version
###############################################################################

if [ -z ${PROJECT_VERSION_MAJOR:+x} ]; then
  PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR_DEFAULT}
fi
if [ -z ${PROJECT_VERSION_MINOR:+x} ]; then
  PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR_DEFAULT}
fi
if [ -z ${PROJECT_VERSION_UPDATE:+x} ]; then
  PROJECT_VERSION_UPDATE=${PROJECT_VERSION_UPDATE_DEFAULT}
fi
PROJECT_VERSION_BASE=${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
if [ -z ${PROJECT_VERSION_UPDATE:+x} ]; then
  PROJECT_VERSION_BASE=${PROJECT_VERSION_BASE}.${PROJECT_VERSION_UPDATE}
fi
PROJECT_VERSION=${PROJECT_VERSION_BASE}
PROJECT_VERSION_SHORT=${PROJECT_VERSION_BASE}
PROJECT_VERSION_PUBLIC=${PROJECT_VERSION_BASE}

if [ ! -z ${BUILD_DEVELOPER:+x} ]; then
  PROJECT_VERSION=${PROJECT_VERSION}-${BUILD_DEVELOPER}
  if [ ! -z ${BUILD_BRANCH:+x} ]; then
    PROJECT_VERSION=${PROJECT_VERSION}-${BUILD_BRANCH}
  fi
fi

if [ -z ${PROJECT_VERSION_TAG:+x} ]; then
  PROJECT_VERSION_TAG=${PROJECT_VERSION_TAG_DEFAULT}
fi
if [ ! -z ${PROJECT_VERSION_TAG:+x} ]; then
  PROJECT_VERSION=${PROJECT_VERSION}-${PROJECT_VERSION_TAG}
  PROJECT_VERSION_PUBLIC=${PROJECT_VERSION_PUBLIC}-${PROJECT_VERSION_TAG}
fi

PROJECT_VERSION_HASH=$("${TOOL_GIT}" rev-parse --short HEAD 2>/dev/nul)
if [ ! -z ${PROJECT_VERSION_HASH:+x} ]; then
  PROJECT_VERSION=${PROJECT_VERSION}-${PROJECT_VERSION_HASH}
  PROJECT_VERSION_SHORT=${PROJECT_VERSION_SHORT}-${PROJECT_VERSION_HASH}
  PROJECT_VERSION_PUBLIC=${PROJECT_VERSION_PUBLIC}-${PROJECT_VERSION_HASH}
fi

printAlign Version: ${PROJECT_VERSION_BASE} "(${PROJECT_VERSION_SHORT})" {${PROJECT_VERSION}}

###############################################################################
# Generate the version header for the project
###############################################################################

PROJECT_VERSION_HEADER=${PROJECT_DIR_BUILD}/Include/ProjectVersion.h

if [ -z ${BUILD_CLEAN:+x} ]; then

  ${TOOL_MKDIR} "$(dirname "${PROJECT_VERSION_HEADER}")" 2>/dev/nul
  RESULT=$?
  if [ ${RESULT} -ne 0 ]; then
    exitScript ${RESULT}
  fi

  printAlign Generating: ${PROJECT_VERSION_HEADER/#${WORKSPACE}\//}

  printf "// Autogenerated ${PROJECT_VERSION_HEADER/#${WORKSPACE}\//}\r\n" > "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_NAME L\"${PROJECT_NAME}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_NAME_A \"${PROJECT_NAME}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_SAFE_NAME L\"${PROJECT_SAFE_NAME}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_SAFE_NAME_A \"${PROJECT_SAFE_NAME}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_VERSION L\"${PROJECT_VERSION}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_VERSION_A \"${PROJECT_VERSION}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_VERSIONCODE PACK_VERSIONCODE_FULL(${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_UPDATE})\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_DATETIME L\"${PROJECT_DATETIME}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_DATETIME_A \"${PROJECT_DATETIME}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_FIRMWARE_VERSION L\"${PROJECT_FIRMWARE_MAJOR}.${PROJECT_FIRMWARE_MINOR}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_FIRMWARE_VERSION_A \"${PROJECT_FIRMWARE_MAJOR}.${PROJECT_FIRMWARE_MINOR}\"\r\n" >> "${PROJECT_VERSION_HEADER}"
  printf "#define _PROJECT_FIRMWARE_VERSIONCODE PACK_VERSIONCODE_FULL(${PROJECT_FIRMWARE_MAJOR}, ${PROJECT_FIRMWARE_MINOR}, ${PROJECT_FIRMWARE_UPDATE})\r\n" >> "${PROJECT_VERSION_HEADER}"

fi

