@echo off
REM Codexium Magnus - Windows CMake Configuration Wrapper
REM This batch file calls the PowerShell configuration script

powershell.exe -ExecutionPolicy Bypass -File "%~dp0configure-windows.ps1" %*
