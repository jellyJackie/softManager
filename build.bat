setlocal

cd /d %~dp0
mkdir vsbuild
cd vsbuild
cmake .. -A Win32
cmake --build . --config Release

endlocal
