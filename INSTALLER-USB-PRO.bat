@echo off
title Clipper Pro - installation par USB (sans navigateur)
cd /d "%~dp0"
rem Sous Windows, Chrome / Edge gardent le port MIDI du SMK-37 pendant son redemarrage en mode mise a jour :
rem l installation peut rester bloquee. Cet installateur parle au clavier directement, par le MIDI de Windows
rem (winmm) : rien a installer, Python seul suffit.
if not exist "build\release\clipper-pro.fwsc" (echo Les paquets ne sont pas compiles : lance d abord INSTALL-CLIPPER-PRO.bat. & pause & exit /b 1)
tasklist /FI "IMAGENAME eq chrome.exe" 2>nul | find /I "chrome.exe" >nul && (echo Chrome est encore ouvert : ferme-le completement ^(toutes les fenetres^), puis relance ce fichier. & pause & exit /b 1)
tasklist /FI "IMAGENAME eq msedge.exe" 2>nul | find /I "msedge.exe" >nul && (echo Edge est encore ouvert : ferme-le completement, puis relance ce fichier. ^(Edge tourne parfois en arriere-plan : Gestionnaire des taches^). & pause & exit /b 1)
echo == Le clavier
python tools\fm1_install.py --info
echo.
echo == Installation : le paquet du modele branche (build\release\)
python tools\fm1_install.py build\release\
echo.
echo Si ca s arrete : rebranche le cable USB et relance ce fichier ^(l ecriture reprend^).
pause
