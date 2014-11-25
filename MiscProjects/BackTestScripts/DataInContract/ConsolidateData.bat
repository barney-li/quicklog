@echo off&setlocal enabledelayedexpansion
set DEST_FOLDER=D:\BACKTESTS\cointegration\
if exist !DEST_FOLDER!*.dat del !DEST_FOLDER!*.dat
mkdir !DEST_FOLDER!
for /D %%a in (*) do (
  echo %%a
  cd %%a
  if exist %%a.txt del %%a.txt
  if exist %%a.dat del %%a.dat
  if exist *.dat (
    call cat *.dat>%%a.txt
    rename %%a.txt %%a.dat
    move %%a.dat !DEST_FOLDER!%%a.dat
  )
  cd ..
)
