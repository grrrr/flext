ifdef SNDOBJ
SRCS+=$(SRCS_SNDOBJ)
HDRS+=$(HDRS_SNDOBJ)
INCPATH+=-I$(SNDOBJ)
LIBS+=-lsndobj
endif

ifdef STK
SRCS+=$(SRCS_STK)
HDRS+=$(HDRS_STK)
INCPATH+=-I$(STK)
LIBS+=-lstk
endif