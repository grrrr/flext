##############################################

LDFLAGS += -dynamic -Wl,-x -framework ApplicationServices -framework vecLib

##############################################

ifdef DEBUG
CFLAGS += -g
else
LDFLAGS += -Wl,-S
endif
