@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Support/SafeName.bat
rem
rem Convert a variable contents to safe for FAT filesystems
rem
rem ===========================================================================

set TU_INPUT=!%~1!
set TU_INPUT=%TU_INPUT:a=A%
set TU_INPUT=%TU_INPUT:b=B%
set TU_INPUT=%TU_INPUT:c=C%
set TU_INPUT=%TU_INPUT:d=D%
set TU_INPUT=%TU_INPUT:e=E%
set TU_INPUT=%TU_INPUT:f=F%
set TU_INPUT=%TU_INPUT:g=G%
set TU_INPUT=%TU_INPUT:h=H%
set TU_INPUT=%TU_INPUT:i=I%
set TU_INPUT=%TU_INPUT:j=J%
set TU_INPUT=%TU_INPUT:k=K%
set TU_INPUT=%TU_INPUT:l=L%
set TU_INPUT=%TU_INPUT:m=M%
set TU_INPUT=%TU_INPUT:n=N%
set TU_INPUT=%TU_INPUT:o=O%
set TU_INPUT=%TU_INPUT:p=P%
set TU_INPUT=%TU_INPUT:q=Q%
set TU_INPUT=%TU_INPUT:r=R%
set TU_INPUT=%TU_INPUT:s=S%
set TU_INPUT=%TU_INPUT:t=T%
set TU_INPUT=%TU_INPUT:u=U%
set TU_INPUT=%TU_INPUT:v=V%
set TU_INPUT=%TU_INPUT:w=W%
set TU_INPUT=%TU_INPUT:x=X%
set TU_INPUT=%TU_INPUT:y=Y%
set TU_INPUT=%TU_INPUT:z=Z%
set TU_INPUT=%TU_INPUT:-=_%
set TU_INPUT=%TU_INPUT: =_%
set %~1=%TU_INPUT%
