SFML_PATH = F:\C++\lib\SFML
SQLITE_PATH = F:\C++\lib\SQLite
CPP_PATH = F:\C++\MSVC\VC\Tools\MSVC\14.11.25503
WINC_PATH = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.16299.0"
WLIB_PATH = "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.16299.0"
SP = .\Source
HP = .\Headers

INCLUDE_PATH = /I$(SFML_PATH)\include /I$(SQLITE_PATH) /I$(CPP_PATH)\include /I$(WINC_PATH)\ucrt /IHeaders

LIB_PATH = /LIBPATH:$(SFML_PATH)\lib /LIBPATH:$(WLIB_PATH)\um\x64 /LIBPATH:$(WLIB_PATH)\ucrt\x64 /LIBPATH:$(CPP_PATH)\lib\x64 /LIBPATH:$(SQLITE_PATH)

WINDOW_APP = /subsystem:windows /entry:mainCRTStartup

LIBS = user32.lib Advapi32.lib \
sqlite3.lib \
winmm.lib \
sfml-system-s.lib \
ws2_32.lib \
sfml-network-s.lib \
openal32.lib flac.lib vorbisenc.lib vorbisfile.lib vorbis.lib ogg.lib \
sfml-audio-s.lib \
opengl32.lib gdi32.lib \
sfml-window-s.lib \
freetype.lib \
sfml-graphics-s.lib

INCLUDES = $(SP)\main.cpp \
$(SP)\md\MeteorItem.cpp \
$(SP)\md\Container.cpp \
$(SP)\md\Folder.cpp \
$(SP)\md\Playlist.cpp \
$(SP)\md\File.cpp

HEADERS = \
$(HP)\md\MeteorItem.hpp \
$(HP)\md\Container.hpp \
$(HP)\md\Folder.hpp \
$(HP)\md\Playlist.hpp \
$(HP)\md\File.hpp

.\bin\Meteoroid.exe: makefile \
$(INCLUDES) $(HEADERS)
	cl /EHsc /MT /Fo.\obj\ /DSFML_STATIC $(INCLUDE_PATH) $(INCLUDES) /link $(LIB_PATH) $(LIBS) /out:bin\Meteoroid.exe
