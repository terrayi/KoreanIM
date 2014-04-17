
NAME=KoreanIM
TYPE=SHARED
SRCS= \
	KoreanInputServerMethod.cpp \
	KoreanInputServerLooper.cpp
RSRCS= 
LIBS= intl hangul _APP_ be
LIBPATHS=
SYSTEM_INCLUDE_PATHS=
LOCAL_INCLUDE_PATHS=
OPTIMIZE=
DEFINES=
WARNINGS=ALL
SYMBOLS=
DEBUGGER=TRUE
COMPILER_FLAGS=
LINKER_FLAGS=

all: _APP_ default

include /boot/develop/etc/makefile-engine

clean ::
	rm -f _APP_

_APP_: /system/servers/input_server
	ln -s $< $@

