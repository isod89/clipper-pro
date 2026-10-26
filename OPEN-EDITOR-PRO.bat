@echo off
title Clipper Pro editor
cd /d "%~dp0"
if not exist build\clipper-pro-site\webapp\editor\index.html (
  echo The editor is not built yet: run INSTALL-CLIPPER-PRO.bat first.
  pause
  exit /b 1
)
start "" http://localhost:8768/webapp/editor/
echo Clipper Pro editor: http://localhost:8768/webapp/editor/  (keep this window open while you use it)
python -m http.server 8768 --bind 127.0.0.1 --directory build/clipper-pro-site
pause
