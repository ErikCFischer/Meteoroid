SFML_PATH = F:\C++\lib\SFML
SQLITE_PATH = F:\C++\lib\SQLite
CPP_PATH = F:\C++\MSVC\VC\Tools\MSVC\14.11.25503
WINC_PATH = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0"
WLIB_PATH = "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0"
SP = .\Source
HP = .\Headers

INCLUDE_PATH = /I$(SFML_PATH)\include /I$(SQLITE_PATH) /I$(CPP_PATH)\include /I$(WINC_PATH)\ucrt /IHeaders

LIB_PATH = /LIBPATH:$(SFML_PATH)\lib /LIBPATH:$(WLIB_PATH)\um\x64 /LIBPATH:$(WLIB_PATH)\ucrt\x64 /LIBPATH:$(CPP_PATH)\lib\x64

WINDOW_APP = /subsystem:windows /entry:mainCRTStartup

LIBS = user32.lib Advapi32.lib \
winmm.lib \
sfml-system-s.lib \
ws2_32.lib \
sfml-network-s.lib \
openal32.lib flac.lib vorbisenc.lib vorbisfile.lib vorbis.lib ogg.lib \
sfml-audio-s.lib \
opengl32.lib gdi32.lib \
sfml-window-s.lib \
freetype.lib jpeg.lib \
sfml-graphics-s.lib

INCLUDES = $(SP)\main.cpp

.\bin\Meteoroid.exe: makefile \
$(SP)\main.cpp
	cl /EHsc /MT $(INCLUDE_PATH) /Fo.\obj\ /DSFML_STATIC $(INCLUDES) /link $(LIB_PATH) $(LIBS) $(SQLITE_PATH)\sqlite3 /out:bin\Meteoroid.exe
