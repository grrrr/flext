ifdef SHARED
EXT=dylib
else
EXT=a
endif

LDFLAGS += -dynamiclib -flat_namespace -undefined suppress
