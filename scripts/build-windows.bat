@echo off
REM Codexium Magnus - Windows Build Wrapper
REM This batch file calls the PowerShell build script

powershell.exe -ExecutionPolicy Bypass -File "%~dp0build-windows.ps1" %*
