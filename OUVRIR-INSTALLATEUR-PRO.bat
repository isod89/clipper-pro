@echo off
title Clipper Pro - installateur web
rem Sans recompiler : refait le site de l installateur avec les paquets deja dans build\release\, puis l ouvre
rem sur http://localhost:8768/webapp/installer/ (garder cette fenetre ouverte pendant l installation).
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-clipper-pro.ps1" -SiteOnly
pause
