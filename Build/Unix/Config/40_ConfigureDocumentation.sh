#! /bin/bash

###############################################################################
#
# @file Build/Unix/Config/40_ConfigureDocumentation.sh
#
# Configure documentation options
#
###############################################################################

BUILD_DOCS=
BUILD_DOCS_FILE=

PROJECT_DIR_DOCS=${PROJECT_DIR_PACKAGE}/Documentation
PROJECT_DIR_SUPPORT_DOCS=${PROJECT_DIR_SUPPORT}/Doxygen
PROJECT_DOCS_INPUT=

TOOL_DOXYGEN=$("${TOOL_WHERE}" doxygen 2>/dev/nul)

DOCS_PUBLIC=

###############################################################################
# Parse script arguments
###############################################################################

COUNTER=1
while [ ${COUNTER} -le $# ]; do
  case "${*:${COUNTER}:1}" in
    "--no-docs")
      BUILD_DOCS=
      ;;
    "--dev-docs")
      BUILD_DOCS=Yes
      ;;
    "--docs")
      BUILD_DOCS=Yes
      DOCS_PUBLIC=Yes
      ;;
    "--dev-docs-only")
      BUILD_DOCS=Yes
      BUILD_DRYRUN=Yes
      ;;
    "--docs-only")
      BUILD_DOCS=Yes
      BUILD_DRYRUN=Yes
      DOCS_PUBLIC=Yes
      ;;
    "--docs-dir")
      let COUNTER+=1
      if [ ${COUNTER} -gt $# ]; then
        break
      fi
      PROJECT_DIR_DOCS=${*:${COUNTER}:1}
      ;;
    "--docs-dir=*")
      PROJECT_DIR_DOCS=${*:${COUNTER}:1}
      PROJECT_DIR_DOCS=${PROJECT_DIR_DOCS:11}
      ;;
    *) ;;
  esac
  let COUNTER+=1
done

###############################################################################
# Configure doxygen
###############################################################################

if [ -z ${TOOL_DOXYGEN:+x} ]; then
  BUILD_DOCS=
  printAlign doxygen: Not found
else
  printAlign doxygen: ${TOOL_DOXYGEN}

  #############################################################################
  # Prepare documentation
  #############################################################################

  if [ -z ${PROJECT_DIR_DOCS:+x} ]; then
    PROJECT_DIR_DOCS=${PROJECT_DIR_PACKAGE}/Documentation
  fi
  if [ -z ${BUILD_DOCS_FILE:+x} ]; then
    BUILD_DOCS_FILE=${PROJECT_DIR_SUPPORT_DOCS}/Doxyfile
  fi
  if [ ! -z ${BUILD_DOCS:+x} ]; then
    printAlign Documentation: ${PROJECT_DIR_DOCS/#${WORKSPACE}\//}
    printAlign Doxyfile: ${BUILD_DOCS_FILE/#${WORKSPACE}\//}
  fi

  PROJECT_DOCS_VERSION=${PROJECT_VERSION_PUBLIC}
  if [ -z ${DOCS_PUBLIC:+x} ]; then
    if [ ! -z ${BUILD_DEVELOPER:+x} ]; then
      PROJECT_DOCS_VERSION=${PROJECT_VERSION}
      PROJECT_DOCS_INPUT="${PROJECT_DOCS_INPUT} \"\$(PROJECT_DIR_SOURCE)/Library\" \"\$(PROJECT_DIR_SOURCE)/Application\""
    fi
  fi

fi

