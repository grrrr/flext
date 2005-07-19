DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src

# trick PD in a way that only the cygwin pthread headers are used
# CFLAGS += -DPTHREAD_H -include /usr/include/sched.h -include /usr/include/pthread.h 

LIBS += $(PDPATH)/bin/pd.dll 
LIBS += $(PDPATH)/bin/pthreadVC.dll
