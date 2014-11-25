@echo off&setlocal enabledelayedexpansion
for /D  %%a in (*) do (
	set TempPath=!cd!\%%a
	call MoveData.bat !TempPath! %%a
	
)