DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src

# trick PD in a way that only the cygwin pthread headers are used
CFLAGS += -include /usr/include/sched.h -include /usr/include/pthread.h -DPTHREAD_H

LIBS += $(PDPATH)/bin/pd.dll 
#LIBS += $(PDPATH)/bin/pthreadVC.dll
