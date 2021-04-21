rmdir /q /s ..\\Intermediate
rmdir /q /s ..\\Saved\\Autosaves
rmdir /q /s ..\\Saved\\Backup
rmdir /q /s ..\\Saved\\Collections
rmdir /q /s ..\\Saved\\Config
rmdir /q /s ..\\Saved\\Crashes
rmdir /q /s ..\\Saved\\Logs

rmdir /q /s ..\\.vs
rmdir /q /s ..\\Binaries
rmdir /q /s ..\\DerivedDataCache

rmdir /q /s ..\\Plugins\\Sketch\\Binaries
rmdir /q /s ..\\Plugins\\Sketch\\Intermediate
rmdir /q /s ..\\Plugins\\MeshDistortion\\Binaries
rmdir /q /s ..\\Plugins\\MeshDistortion\\Intermediate
rmdir /q /s ..\\Plugins\\VolumetricClouds\\Binaries
rmdir /q /s ..\\Plugins\\VolumetricClouds\\Intermediate

del ..\\TPSProject.sln

pause
exit 0
