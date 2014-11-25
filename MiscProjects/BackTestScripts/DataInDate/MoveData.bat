@echo off&setlocal enabledelayedexpansion
set OldDir=!cd!
set DataInContract="D:\Tests\Data\DataInContract"
cd %1
set DataDir=!cd!
for /R %%f in (*.dat) do (
	set ContractName=%%~nf
	md "!DataInContract!\!ContractName!"
	copy %%f "!DataInContract!\!ContractName!\%2.dat"
)
cd !OldDir!