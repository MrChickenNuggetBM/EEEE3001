@echo off
setlocal enabledelayedexpansion
echo:

echo --------------------------------------------------- git status -------------------------------------------------
@echo git status
git status
echo:

echo --------------------------------------------------- git add * --------------------------------------------------
set /p "file=Please enter the file names (* for all files): "
@echo git add %file%
git add %file%
echo:

echo --------------------------------------------------- git status -------------------------------------------------
@echo git status
git status

echo --------------------------------------------------- git commit -------------------------------------------------
set "file=version.txt"
if not exist !file! echo 1>!file!
set /p X=<version.txt
set /a X+=1
echo !X!>!file!
set /p msg="Please enter the commit message, or press enter for auto message "
if /i "%msg%"=="" (
    @echo git commit -m "v!X!"
    git commit -m "v!X!"
) else (
    @echo git commit -m "%msg%"
    git commit -m "%msg%"
)
echo:

echo --------------------------------------------------- git push ---------------------------------------------------
set /p decision="Would you like to push (y/n)?"
IF /i "%decision%"=="y" (
    echo Pushing...
) ELSE (
    IF /i "%decision%"=="n" (
        echo Did not push
        exit /B
    ) ELSE (
        echo Incorrect input. Aborting...
        exit /B
    )
)
@echo git push
git push
echo:

:: Display the incremented version
echo Updated to version !X!
if not "!msg!"=="" (
    echo Commit Message: "!msg!"
)

endlocal
