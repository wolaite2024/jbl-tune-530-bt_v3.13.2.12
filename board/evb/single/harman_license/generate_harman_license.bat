echo off

python %cd%\harman_license.py

set MP_INI=%cd%\mp.ini
set PREPEND_HEADER=..\..\..\..\tool\Gadgets\prepend_header.exe
set MD5=..\..\..\..\tool\Gadgets\md5.exe
set FILE_NAME=harman_license

%PREPEND_HEADER% /mp_ini %MP_INI% /backup_data1 %cd%\%FILE_NAME%.bin

%MD5% %cd%\%FILE_NAME%_MP.bin
del harman_license.bin
del harman_license_MP.bin

pause

echo on
