@echo off
title Clipper Pro installer
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-clipper-pro.ps1" %*
pause
