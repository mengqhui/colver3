@echo off
rem ===========================================================================
rem Remove quotes from a variable
rem ===========================================================================

set QUOTED_VAR=%~1
if not defined QUOTED_VAR goto:eof
shift
set %QUOTED_VAR%=

:putNextValue

  set QUOTED_VALUE=%~1
  if not defined QUOTED_VALUE goto:eof
  if not defined %QUOTED_VAR% (
    set %QUOTED_VAR%=%QUOTED_VALUE%
  ) else (
    set %QUOTED_VAR%=!%QUOTED_VAR%! %QUOTED_VALUE%
  )
  shift
  goto putNextValue

