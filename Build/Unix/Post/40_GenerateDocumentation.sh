#! /bin/bash

###############################################################################
#
# @file Build/Unix/Post/40_GenerateDocumentation.sh
#
# Generate documentation
#
###############################################################################

if [ ! -z ${BUILD_DOCS:+x} ]; then
  if [ ! -z ${BUILD_DOCS_FILE:+x} ]; then
    if [ ! -z PROJECT_DIR_DOCS:+x} ]; then
      if [ ! -z TOOL_DOXYGEN:+x} ]; then
        ${TOOL_MKDIR} "${PROJECT_DIR_DOCS}" 2>&1
        if [ ! -d "${PROJECT_DIR_DOCS}" ]; then
          exitScript 1
        fi
        "${TOOL_DOXYGEN}" "${BUILD_DOCS_FILE}" 2>&1
      fi
    fi
  fi
fi

