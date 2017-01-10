
' Date and time tool for retrieving the current date and time

Dim dNow, sDate, sMonth, sDay, sHour, sMinute, sSecond

' Get the current date and time
dNow = Now()

' Get the current month and pad if needed
sMonth = Month(dNow)
if sMonth < 10 then
  sMonth = "0" & sMonth
end if

' Get the current day of month and pad if needed
sDay = Day(dNow)
if sDay < 10 then
  sDay = "0" & sDay
end if

' Set the full date
sDate = Year(dNow) & "-" & sMonth & "-" & sDay

' Get the current hours in 24 hour format
sHour = Hour(dNow)
if sHour < 10 then
  sHour = "0" & sHour
end if

' Get the current minutes
sMinute = Minute(dNow)
if sMinute < 10 then
  sMinute = "0" & sMinute
end if

' Get the current seconds
sSecond = Second(dNow)
if sSecond < 10 then
  sSecond = "0" & sSecond
end if

' Set the full time
sTime = sHour & ":" & sMinute & ":" & sSecond

' Output the current date and time
WScript.Echo sDate & " " & sTime

