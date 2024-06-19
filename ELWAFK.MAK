###############################################################################
#####               This makefile generates ELWAFK.DLL                   #####
###############################################################################
!include $(WGDEV)\src\gcrules.mak

DLLLIB =            \
     $(DLLLIB)      \
     wgserver.lib   \
     galgsbl.lib

OBJFILES =          \
     elwafk.obj     

elwafk.dll:         \
     $(GCBUILD)     \
     elwafk.h       \
     $(OBJFILES)    \
     $(DLLLIB)      \
     $(DEFILE)
     $(LINK) $(LIBCMD) @&&|
$(PRODLL) $(OBJFILES)
$(RUNDIR)\$&
$(RUNDIR)\$&
$(DLLLIB) $(LINKDLL)
$(DEFILE)
|
