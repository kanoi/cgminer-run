@echo off
REM cgminer
:Loop

Set NOW=%date:~10,4%%date:~7,2%%date:~4,2%.%time:~0,2%%time:~3,2%%time:~6,2%

REM This will load cgminer.conf if it is in the current directory
REM and of course you can add options here - see README.txt
.\cgminer.exe --api-listen --api-allow W:127.0.0.1 2> "run.%NOW%.log"

echo Sleeping for 5 seconds
ping -n 5 127.0.0.1 > NUL
GOTO Loop
