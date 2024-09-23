SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files\Common Files\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;C:\Program Files\TortoiseGit\bin;D:\Program Files\Altium Deigner Summer 09\System;C:\mingw32\bin;D:\Qt\4.8.6\bin;D:\Qt\qtcreator-2.5.2\bin;D:\Program Files\Git\cmd;C:\Program Files\Java\jdk-22\bin;C:\Users\g__mo\esp\tool\tools\cmake\3.24.0\bin;D:\work_place\ESP_repository\tools\esptool-master;D:\work_place\ESP_repository\tools\tools\idf-exe\1.0.3;D:\Program Files\Python3.8.1\Scripts\;D:\Program Files\Python3.8.1\;C:\Users\g__mo\AppData\Local\Microsoft\WindowsApps;D:\Tool\Android\platform-tools;C:\Windows\System32\Wbem;C:\Users\g__mo\AppData\Local\Programs\Microsoft VS Code\bin;C:\Users\g__mo\esp\tool\tools\cmake\3.24.0\bin;D:\work_place\ESP_repository;D:\Tool\Android\PhoenixSuit_v1.13\
SET CPU_TYPE=STM32F103RC
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=inverjet_boot
SET CPU_CLOCK=0x007A1200
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\inverjet_boot\startup_stm32f103xe._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\iap.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stmflash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\sys.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_msp.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_timebase_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_gpio_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_tim_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_rcc_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_dma.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_cortex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_pwr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_flash_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\stm32f1xx_hal_exti.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\system_stm32f1xx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\inverjet_boot\mbcrc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\inverjet_boot\inverjet_boot.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\inverjet_boot\inverjet_boot.axf" --i32combined --output ".\inverjet_boot\inverjet_boot.hex"
fromelf --bin --output=.\Output\inverjet_boot.bin .\inverjet_boot\inverjet_boot.axf
fromelf --bin --output=..\..\inverjet_app\Bin\inverjet_boot.bin .\inverjet_boot\inverjet_boot.axf
