@echo off
set flash_bank_path=%1
set ic_type=%2

if exist .\flash_map.h  del .\flash_map.h
copy ..\..\..\bin\%ic_type%\flash_map_config\flash_%flash_bank_path%\flash_map.h ..\..\..\bin\%ic_type%\flash_map_config\flash_map.h
copy ..\..\..\bin\%ic_type%\flash_map_config\flash_%flash_bank_path%\flash_map.ini ..\..\..\bin\%ic_type%\flash_map_config\flash_map.ini

copy ..\..\..\bin\%ic_type%\leaudio\leaudio.lib ..\..\..\bin\%ic_type%\leaudio.lib
copy ..\..\..\bin\%ic_type%\framework\framework_stereo.lib ..\..\..\bin\%ic_type%\framework.lib
copy ..\..\..\bin\%ic_type%\ble_mgr\ble_mgr_all.lib ..\..\..\bin\%ic_type%\ble_mgr.lib
