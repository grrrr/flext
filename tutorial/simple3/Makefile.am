#
# automake template
# added by tim blechmann
#

NAME = simple3

BUILT_SOURCES = main.cpp

EXTRA_DIST = main.cpp \
	$(NAME).cw \
	$(NAME).dsp \
	$(NAME).vcproj

CXXFLAGS  = @CXXFLAGS@ \
	@OPT_FLAGS@ \
	@INCLUDEDIR@ \
	-I../../source \
	$(DEFS)

LDFLAGS = @LDFLAGS@ \
	$(patsubst %,-framework %,$(FRAMEWORKS))

LIBS = @LIBS@

FRAMEWORKS = @FRAMEWORKS@

TARGETDIR = @TARGETDIR@

TARGET =$(NAME).@EXTENSION@

OBJECTS = $(patsubst %.cpp,./%.@OBJEXT@,$(BUILT_SOURCES))

SYSDIR = @SYSDIR@


# ----------------------------- targets --------------------------------

all-local: $(OBJECTS)
	$(CXX) $(LDFLAGS) -shared ./*.@OBJEXT@ $(LIBS) -o ../$(TARGETDIR)/$(TARGET)
	strip --strip-unneeded ../$(TARGETDIR)/$(TARGET)

./%.@OBJEXT@ : %.cpp 
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean-local:
	rm -f ../$(TARGETDIR)/$(TARGET)
	rm -f ./$(OBJECTS)

install-exec-local:
	install ../$(TARGET) $(SYSDIR)extra