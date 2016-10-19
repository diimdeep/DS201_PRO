## DS201 PRO Firmware

### Build

Use IAR Embedded Workbench for ARM version 6.40.2

### Flash to device

`Hold Down + turn On` device, connect via USB
Drop Firware.hex file to device, if file changes to Firware.rdy - DONE, otherwise FAIL

### Info

Device info mode: `Hold Ok + turn On`
FW update mode: `Hold Down + turn On`

- http://www.minidso.com/forum.php?mod=viewthread&tid=419
- http://www.seeedstudio.com/wiki/DSO_Nano
- http://www.seeedstudio.com/wiki/DSO_Nano_v3

From some forum:

```
There are many DSO Nano versions..

Version 1. DS201/DSO Nano v1
Version 2. DS201/DSO Nano v2 old LCD
Version 3. DS201/DSO Nano v2 new LCD
Version 4. DS201/DSO Nano v3-external mSD slot
Version 5. DS201/DSO Nano v3-2MBytes built in memory
Version 6. DS201 PRO/DSO Nano v3-8MBytes built in memory

Depending what version you get will dictate if you can have a fully functional Benf 3.64 or not. Since Benf firmware was written for external MicroSD in mind and the old LCD, it will work on versions 1 and 2 just fine. Then you can load Benf 3.64 patched v3 on version 3 and 4, again fully functional. It will also run ok on 5 to 6 versions minus the Saving/Loading functionality. Some people did get Benf working fully on version 5 to 6, but it requires soldering in the external MicroSD slot/card.

I personally own the DSO Nano v2 old LCD with Electron 007 firmware and it works for me but I am droolling over DS203. Therefore I will recommend getting DS203/DSO QUAD hw v2.81+ and load Wildcat 6 on it. 
```
