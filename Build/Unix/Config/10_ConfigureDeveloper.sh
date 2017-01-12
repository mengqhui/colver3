#! /bin/bash

###############################################################################
#
# @file Build/Unix/Config/10_ConfigureDeveloper.sh
#
# Configure developer options
#
###############################################################################

BUILD_DEVELOPER=
BUILD_BRANCH=

###############################################################################
# Configure git
###############################################################################

TOOL_GIT=$("${TOOL_WHERE}" git 2>/dev/nul)

if [ -z ${TOOL_GIT:+x} ]; then
  printAlign git: Not found
else
  printAlign git: ${TOOL_GIT}

  #############################################################################
  # Get developer
  #############################################################################

  BUILD_DEVELOPER=$("${TOOL_GIT}" config user.name 2>/dev/nul)
  if [ "$("${TOOL_GIT}" log --max-count=1 --author="${DEVELOPER}" 2>/dev/nul)" == "" ]; then
    BUILD_DEVELOPER=${DEVELOPER}
  fi
  if [ -z ${BUILD_DEVELOPER:+x} ]; then
    printAlign Developer: Not a development build
  else
    printAlign Developer: ${BUILD_DEVELOPER}
  fi

  #############################################################################
  # Get branch
  #############################################################################

  BUILD_BRANCH=$("${TOOL_GIT}" rev-parse --abbrev-ref HEAD 2>/dev/nul)
  if [ "${BUILD_BRANCH}" == "master" ]; then
    BUILD_BRANCH=
  fi
  if [ ! -z ${BUILD_BRANCH:+x} ]; then
    printAlign Branch: ${BUILD_BRANCH}
  fi

fi
