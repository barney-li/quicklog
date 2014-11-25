@echo off&setlocal enabledelayedexpansion
for /D %%a in (D:\Tests\Data\m8\*) do (
  echo %%a
  del /Q data\MarketData\*
  copy /Y %%a\cu*.dat .\data\MarketData\
  check-oppo-181.exe
)