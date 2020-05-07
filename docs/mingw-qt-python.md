## Qt MXE
download from https://github.com/mxe/mxe
```
make MXE_TARGETS=x86_64-w64-mingw32.static qt5
```

## Python static library for crosscompiling for windows
```
echo "EXPORTS" > python27.def
x86_64-w64-mingw32.static-nm python27.lib | grep " T _" | sed "s/.* T _//" >> python27.def
x86_64-w64-mingw32.static-dlltool --input-def python27.def --dllname python27 --output-lib libpython27.a
```
