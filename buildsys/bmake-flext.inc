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


!ifdef FLEXTBUILD
DEFS=$(DEFS) /DFLEXT_EXPORTS
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
FLEXTNAME=flext-$(RTSYS)_d
!else
FLEXTNAME=flext-$(RTSYS)
!endif
!endif

!endif # SHARED

##############################################
# build subdirectories
##############################################

!ifdef FLEXTBUILD

!ifdef SHARED

!ifdef DEBUG
OUTSUB=dd
!else
OUTSUB=dr
!endif

!else # SHARED

!ifdef THREADED
!ifdef DEBUG
OUTSUB=td
!else
OUTSUB=tr
!endif
!else
!ifdef DEBUG
OUTSUB=sd
!else
OUTSUB=sr
!endif
!endif

!endif # SHARED

!else # FLEXTBUILD
OUTSUB=.
!endif # FLEXTBUILD

