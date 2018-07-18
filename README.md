# JLauncher
A windows executable that will search for the currently installed java (the one in your %PATH%), and use it to launch a Jar with the same name as the executable and at the same place (so that renaming JLauncher.exe to JRomManager.exe, then execute, it will then launch `javaw -Xmx1g -jar .\JRomManager.jar`)
It will also detect if JVM is 64bits, if so it will use -Xmx2g instead of -Xmx1g (not currently configurable)
