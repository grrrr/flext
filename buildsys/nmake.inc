##############################################
# flext defines
##############################################

!ifdef SHARED
# --- shared ---
DEFS=$(DEFS) /DFLEXT_SHARED

!elseifdef THREADED
# --- static multi-threaded ---
DEFS=$(DEFS) /DFLEXT_THREADS

!else 
# --- static single-threaded ---

!endif

##############################################
# name of flext library
##############################################

!ifdef SHARED

!ifdef DEBUG
FLEXTNAME=flext-$(RTSYS)_d
!else
FLEXTNAME=flext-$(RTSYS)
!endif

!else

!ifdef THREADED
!ifdef DEBUG
FLEXTNAME=flext-$(RTSYS)_td
!else
FLEXTNAME=flext-$(RTSYS)_t
!endif
!else
!ifdef DEBUG
FLEXTNAME=flext-$(RTSYS)_sd
!else
FLEXTNAME=flext-$(RTSYS)_s
!endif
!endif

!endif # SHARED

##############################################
# product name and folder
##############################################

!ifdef DEBUG
TARGETMODE=debug
!else
TARGETMODE=release
!endif

!ifdef SHARED
TARGETTYPE=shared
!else
!ifdef THREADED
TARGETTYPE=multi
!else
TARGETTYPE=single
!endif
!endif

OUTSUB=$(TARGETMODE)-$(TARGETTYPE)
