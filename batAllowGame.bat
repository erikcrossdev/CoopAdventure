@echo off
REM === CONFIG ===
set UE_EDITOR="C:\Program Files\Epic Games\UE_5.2\Engine\Binaries\Win64\UnrealEditor.exe"
set PORT=7777

echo.
echo === Adicionando exceção para UnrealEditor.exe no Firewall ===
netsh advfirewall firewall add rule name="UnrealEditor" dir=in action=allow program=%UE_EDITOR% enable=yes

echo.
echo === Liberando porta UDP %PORT% ===
netsh advfirewall firewall add rule name="UnrealEditor_UDP_%PORT%" dir=in action=allow protocol=UDP localport=%PORT%

echo.
echo === Liberando porta TCP %PORT% ===
netsh advfirewall firewall add rule name="UnrealEditor_TCP_%PORT%" dir=in action=allow protocol=TCP localport=%PORT%

echo.
echo === Regras de saída UDP ===
netsh advfirewall firewall add rule name="UnrealEditor_UDP_%PORT%_OUT" dir=out action=allow protocol=UDP localport=%PORT%

echo.
echo === Regras de saída TCP ===
netsh advfirewall firewall add rule name="UnrealEditor_TCP_%PORT%_OUT" dir=out action=allow protocol=TCP localport=%PORT%

echo.
echo === Concluído! Firewall liberado para UnrealEditor ===
pause