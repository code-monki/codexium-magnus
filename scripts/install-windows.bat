@echo off
REM Codexium Magnus - Windows Installation Wrapper
REM This batch file calls the PowerShell installation script

powershell.exe -ExecutionPolicy Bypass -File "%~dp0install-windows.ps1" %*
