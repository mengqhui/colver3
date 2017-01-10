
' Tee tool for printing from standard input to standard output and standard error

Dim sLine

Do Until WScript.StdIn.AtEndOfStream
  sLine = WScript.StdIn.ReadLine
  WScript.StdOut.WriteLine sLine
  WScript.StdErr.WriteLine sLine
Loop

