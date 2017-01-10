
' Script to align first argument

Dim iAlignment

iAlignment = 15

' Quit the script if no arguments

if WScript.Arguments.Count = 0 then
  WScript.Quit()
end if

' Print the first argument

WScript.StdOut.Write WScript.Arguments(0)

' Align the first argument with spaces

for i = Len(WScript.Arguments(0)) to iAlignment
  WScript.StdOut.Write " "
next

' Print remaining arguments

for i = 1 to WScript.Arguments.Count - 1
  WScript.StdOut.Write " " & WScript.Arguments(i)
next

' Write new line

WScript.StdOut.WriteLine

