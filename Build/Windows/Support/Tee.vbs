'
' @file Build/Windows/Support/Tee.vbs
'
' Tee tool for printing from standard input to standard output and standard error
'

Dim sLine

' Output to standard out and log file
Do Until WScript.StdIn.AtEndOfStream
  sLine = WScript.StdIn.ReadLine
  WScript.StdOut.WriteLine sLine
  WScript.StdErr.WriteLine sLine
Loop

