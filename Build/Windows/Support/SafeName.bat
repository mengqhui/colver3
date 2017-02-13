@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Support/SafeName.bat
rem
rem Convert a variable contents to safe for FAT filesystems
rem
rem ===========================================================================

set TU_INPUT=!%~1!
set TU_INPUT=%TU_INPUT:A=a%
set TU_INPUT=%TU_INPUT:B=b%
set TU_INPUT=%TU_INPUT:C=c%
set TU_INPUT=%TU_INPUT:D=d%
set TU_INPUT=%TU_INPUT:E=e%
set TU_INPUT=%TU_INPUT:F=f%
set TU_INPUT=%TU_INPUT:G=g%
set TU_INPUT=%TU_INPUT:H=h%
set TU_INPUT=%TU_INPUT:I=i%
set TU_INPUT=%TU_INPUT:J=j%
set TU_INPUT=%TU_INPUT:K=k%
set TU_INPUT=%TU_INPUT:L=l%
set TU_INPUT=%TU_INPUT:M=m%
set TU_INPUT=%TU_INPUT:N=n%
set TU_INPUT=%TU_INPUT:O=o%
set TU_INPUT=%TU_INPUT:P=p%
set TU_INPUT=%TU_INPUT:Q=q%
set TU_INPUT=%TU_INPUT:R=r%
set TU_INPUT=%TU_INPUT:S=s%
set TU_INPUT=%TU_INPUT:T=t%
set TU_INPUT=%TU_INPUT:U=u%
set TU_INPUT=%TU_INPUT:V=v%
set TU_INPUT=%TU_INPUT:W=w%
set TU_INPUT=%TU_INPUT:X=x%
set TU_INPUT=%TU_INPUT:Y=y%
set TU_INPUT=%TU_INPUT:Z=z%
set TU_INPUT=%TU_INPUT:-=_%
set TU_INPUT=%TU_INPUT: =_%
set %~1=%TU_INPUT%
