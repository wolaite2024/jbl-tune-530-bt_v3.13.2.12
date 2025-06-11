echo off

set FROMELF_EXE=%1
set AXF_FILE=%2
set TARGET_NAME=%3
set TARGET_BANK=%4
set TARGET_DIR=%5
set IC_TYPE=%6

set TARGET_MAP_PATH=Listings
set TARGET_OBJ_PATH=Objects

set FLASH_MAP_INI=..\..\..\bin\%IC_TYPE%\flash_map_config\flash_map.ini
set FLASH_MAP_H=..\..\..\bin\%IC_TYPE%\flash_map_config\flash_map.h
set HEX_IMAGE=app_image.hex
set MP_INI=mp.ini
set AES_KEY=..\..\..\tool\Gadgets\aes_key.bin
set RTK_RSA=..\..\..\tool\Gadgets\default_rsa_key.pem


set PREPEND_HEADER=..\..\..\tool\Gadgets\prepend_header.exe
set BUILD_DEBUG_TABLE=..\..\..\tool\Gadgets\build_debug_table_tool\build_debug_table.exe
set MD5=..\..\..\tool\Gadgets\md5_generate.sh
REM set MD5=..\..\..\tool\Gadgets\md5.exe
set BIN2HEX=..\..\..\tool\Gadgets\bin2hex.bat
set SREC_CAT=..\..\..\tool\Gadgets\srec_cat.exe

rd /s /q "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%"
mkdir "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%"

rem generate bin and disasm from axf
%FROMELF_EXE%  --bin -o "bin/%IC_TYPE%/%TARGET_DIR%/%TARGET_BANK%/" %AXF_FILE%
copy "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%.bin" "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin"
copy "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%.trace" "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.trace"
del "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%.bin"
del "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%.trace"

rem generate MP bin for bank0

"%PREPEND_HEADER%"  /app_code "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin"
REM "%PREPEND_HEADER%" /aes_key "%AES_KEY%" /app_code "%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin"
"%PREPEND_HEADER%" /rsa "%RTK_RSA%" /app_code "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin"

"%PREPEND_HEADER%" /mp_ini %MP_INI% /app_code "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin"
bash %MD5% "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%_MP.bin"
if exist "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%_MP.bin" del "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%_MP.bin"

copy "%TARGET_OBJ_PATH%\%TARGET_NAME%.htm" "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.htm"
copy "%TARGET_MAP_PATH%\%TARGET_NAME%.map" "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.map"

%BUILD_DEBUG_TABLE% --debug_dir "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%" --out_dir "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%"

REM generate hex for keil download
:: Get Bank0 & Bank1 App start address
for /f "tokens=2,3" %%i in (%FLASH_MAP_H%) do (
    if %%i==BANK0_APP_ADDR set BANK0_APP_ADDR_CFG=%%j
    if %%i==BANK1_APP_ADDR set BANK1_APP_ADDR_CFG=%%j
)

:: Select right app address for bank
if %TARGET_BANK% == bank1 (
    set APP_ADDR_CFG=%BANK1_APP_ADDR_CFG%
) else (
    set APP_ADDR_CFG=%BANK0_APP_ADDR_CFG%
)

:: Convert image from binary to Intel HEX format
%SREC_CAT% "bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin" -binary -offset %APP_ADDR_CFG% -o "%TARGET_OBJ_PATH%\%HEX_IMAGE%" -intel

echo Output Image for Keil Download: "%TARGET_OBJ_PATH%\%HEX_IMAGE%"
echo Build Bank: %TARGET_BANK%
echo Download Address: %APP_ADDR_CFG%

for /f "tokens=1, 2, 3,4" %%i in ('dir /a-d bin\%IC_TYPE%\%TARGET_DIR%\%TARGET_BANK%\%TARGET_NAME%_%TARGET_BANK%.bin') do (
    if %%l == %TARGET_NAME%_%TARGET_BANK%.bin echo %%i %%j %%k %%l
)

echo on
