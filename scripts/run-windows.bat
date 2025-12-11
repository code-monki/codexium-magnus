@echo off
REM Codexium Magnus - Windows Run Wrapper
REM This batch file calls the PowerShell run script

powershell.exe -ExecutionPolicy Bypass -File "%~dp0run-windows.ps1" %*
