#
# automake template
# added by tim blechmann
#

NAME = simple1

BUILT_SOURCES = main.cpp

#EXTRA_DIST = main.cpp

CXXFLAGS  = @CXXFLAGS@ \
	@OPT_FLAGS@ \
	$(patsubst %,-I%,@INCLUDEDIRS@) \
	-I../../source \
    -D FLEXT_INLINE=1 \
	$(DEFS) \
	-DFLEXT_SHARED

LDFLAGS = @LD_FLAGS@ $(patsubst %,-L%,@LIBDIRS@)  $(patsubst %,-framework %,$(FRAMEWORKS))

FRAMEWORKS = @FRAMEWORKS@

#TARGETDIR = @TARGETDIR@

TARGET =$(NAME).@EXTENSION@

OBJECTS = $(patsubst %.cpp,./%.@OBJEXT@,$(BUILT_SOURCES))

SYSDIR = @SYSDIR@


# ----------------------------- targets --------------------------------

all-local: $(OBJECTS)
	$(CXX) $(LDFLAGS) ./*.@OBJEXT@ $(LIBS) -o ./$(TARGET)
	strip -x ./$(TARGET)

./%.@OBJEXT@ : %.cpp 
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean-local:
	rm -f ../$(TARGETDIR)/$(TARGET)
	rm -f ./$(OBJECTS)

install-exec-local:
	install ../$(TARGET) $(SYSDIR)/extra

