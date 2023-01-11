OD_EXE_STRIPPED := scummvm_stripped$(EXEEXT)
bundle = sd-root

ifeq ($(OPENDINGUX_TARGET), miyoomini)
G2X_CATEGORY = App
else
G2X_CATEGORY = games
endif

all: $(OD_EXE_STRIPPED)

$(OD_EXE_STRIPPED): $(EXECUTABLE)
	$(STRIP) $< -o $@

$(bundle): all
	$(MKDIR) -p $(bundle)/$(G2X_CATEGORY)/scummvm
	$(CP) $(DIST_FILES_DOCS) $(bundle)/$(G2X_CATEGORY)/scummvm
	$(MKDIR) $(bundle)/$(G2X_CATEGORY)/scummvm/themes
	$(CP) $(DIST_FILES_THEMES) $(bundle)/$(G2X_CATEGORY)/scummvm/themes/

ifdef DIST_FILES_ENGINEDATA
	$(MKDIR) $(bundle)/$(G2X_CATEGORY)/scummvm/engine-data
	$(CP) $(DIST_FILES_ENGINEDATA) $(bundle)/$(G2X_CATEGORY)/scummvm/engine-data/
endif
ifdef DIST_FILES_NETWORKING
	$(CP) $(DIST_FILES_NETWORKING) $(bundle)/$(G2X_CATEGORY)/scummvm
endif
ifdef DIST_FILES_VKEYBD
	$(CP) $(DIST_FILES_VKEYBD) $(bundle)/$(G2X_CATEGORY)/scummvm
endif
ifdef DYNAMIC_MODULES
	$(MKDIR) $(bundle)/$(G2X_CATEGORY)/scummvm/plugins/
	$(CP) $(PLUGINS) $(bundle)/$(G2X_CATEGORY)/scummvm/plugins/
endif
	$(CP) $(EXECUTABLE) $(bundle)/$(G2X_CATEGORY)/scummvm/scummvm

	$(CP) $(srcdir)/dists/opendingux/scummvm.png $(bundle)/$(G2X_CATEGORY)/scummvm
	$(CP) $(srcdir)/backends/platform/sdl/opendingux/README.OPENDINGUX $(bundle)/$(G2X_CATEGORY)/scummvm/README.man.txt
	echo >> $(bundle)/$(G2X_CATEGORY)/scummvm/README.man.txt
	echo '[General README]' >> $(bundle)/$(G2X_CATEGORY)/scummvm/README.man.txt
	echo >> $(bundle)/$(G2X_CATEGORY)/scummvm/README.man.txt
	cat $(srcdir)/README.md | sed -e 's/\[/⟦/g' -e 's/\]/⟧/g' -e '/^1\.1)/,$$ s/^[0-9][0-9]*\.[0-9][0-9]*.*/\[&\]/' >> $(bundle)/$(G2X_CATEGORY)/scummvm/README.man.txt
	echo '[General README]' >> $(bundle)/$(G2X_CATEGORY)/scummvm/README.man.txt
ifeq ($(OPENDINGUX_TARGET), miyoomini)
	$(CP) $(srcdir)/dists/opendingux/launch.miyoomini.sh $(bundle)/$(G2X_CATEGORY)/scummvm/launch.sh
	$(CP) $(srcdir)/dists/opendingux/config.miyoomini.json $(bundle)/$(G2X_CATEGORY)/scummvm/config.json
else
	$(MKDIR) -p $(bundle)/gmenu2x/sections/games
	$(CP) $(srcdir)/dists/opendingux/scummvm.miyoo $(bundle)/gmenu2x/sections/games/scummvm
endif
	$(STRIP) $(bundle)/$(G2X_CATEGORY)/scummvm/scummvm

sd-zip: $(bundle)
	cd $(bundle) && zip -r ../miyoo.zip $(G2X_CATEGORY) gmenu2x
