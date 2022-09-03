
call :convert deer1
call :convert deer2
call :convert deer3
call :convert deer4


exit /b
:convert
C:\_PurpsStash\PROGRAMMING\BZZRE\TOOLS\utils\qoiconv.exe %1.png %1.qoi
exit /b