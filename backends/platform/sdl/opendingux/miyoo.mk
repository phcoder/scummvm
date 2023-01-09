OD_EXE_STRIPPED := scummvm_stripped$(EXEEXT)
bundle = sd-root

all: $(OD_EXE_STRIPPED)

$(OD_EXE_STRIPPED): $(EXECUTABLE)
	$(STRIP) $< -o $@

$(bundle): all
	$(MKDIR) $(bundle)
	$(MKDIR) $(bundle)/games
	$(MKDIR) $(bundle)/games/scummvm
	$(CP) $(DIST_FILES_DOCS) $(bundle)/games/scummvm
	$(MKDIR) $(bundle)/games/scummvm/themes
	$(CP) $(DIST_FILES_THEMES) $(bundle)/games/scummvm/themes/

ifdef DIST_FILES_ENGINEDATA
	$(MKDIR) $(bundle)/games/scummvm/engine-data
	$(CP) $(DIST_FILES_ENGINEDATA) $(bundle)/games/scummvm/engine-data/
ifeq ($(OPENDINGUX_TARGET), rg99)
	$(CP) $(srcdir)/dists/opendingux/fonts_mini.dat $(bundle)/games/scummvm/engine-data/fonts.dat
endif
endif
ifdef DIST_FILES_NETWORKING
	$(CP) $(DIST_FILES_NETWORKING) $(bundle)/games/scummvm
endif
ifdef DIST_FILES_VKEYBD
	$(CP) $(DIST_FILES_VKEYBD) $(bundle)/games/scummvm
endif
ifdef DYNAMIC_MODULES
	$(MKDIR) $(bundle)/plugins
	$(CP) $(PLUGINS) $(bundle)/games/scummvm/plugins/
endif
	$(CP) $(EXECUTABLE) $(bundle)/games/scummvm/scummvm

	$(CP) $(srcdir)/dists/opendingux/scummvm.png $(bundle)/games/scummvm
	$(CP) $(srcdir)/backends/platform/sdl/opendingux/README.OPENDINGUX $(bundle)/games/scummvm/README.man.txt
	echo >> $(bundle)/games/scummvm/README.man.txt
	echo '[General README]' >> $(bundle)/games/scummvm/README.man.txt
	echo >> $(bundle)/games/scummvm/README.man.txt
	cat $(srcdir)/README.md | sed -e 's/\[/⟦/g' -e 's/\]/⟧/g' -e '/^1\.1)/,$$ s/^[0-9][0-9]*\.[0-9][0-9]*.*/\[&\]/' >> $(bundle)/games/scummvm/README.man.txt
	echo '[General README]' >> $(bundle)/games/scummvm/README.man.txt
	$(MKDIR) $(bundle)/gmenu2x
	$(MKDIR) $(bundle)/gmenu2x/sections
	$(MKDIR) $(bundle)/gmenu2x/sections/games
	$(CP) $(srcdir)/backends/platform/sdl/opendingux/scummvm.miyoo $(bundle)/gmenu2x/sections/games/scummvm
	$(STRIP) $(bundle)/games/scummvm/scummvm

sd-zip: $(bundle)
	cd $(bundle) && zip -r ../miyoo.zip games gmenu2x
