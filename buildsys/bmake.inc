##############################################
# flext defines
##############################################

!ifdef SHARED
# --- shared ---
DEFS=$(DEFS) /DFLEXT_SHARED

!else
!ifdef THREADED
# --- static multi-threaded ---
DEFS=$(DEFS) /DFLEXT_THREADS

!else 
# --- static single-threaded ---

!endif
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

OUTNAME=$(NAME)
OUTSUB=$(TARGETMODE)-$(TARGETTYPE)
