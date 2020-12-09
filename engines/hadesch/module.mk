MODULE := engines/hadesch

MODULE_OBJS = \
    metaengine.o \
    pod_file.o \
    tag_file.o \
    pod_image.o \
    video.o \
    hadesch.o \
    baptr.o \
    rooms/olympus.o \
    rooms/walloffame.o \
    rooms/argo.o \
    rooms/crete.o \
    rooms/minos.o \
    rooms/daedalus.o \
    rooms/seriphos.o \
    rooms/medisle.o \
    rooms/troy.o \
    rooms/quiz.o \
    rooms/minotaur.o \
    rooms/catacombs.o \
    rooms/priam.o \
    rooms/athena.o \
    rooms/volcano.o \
    rooms/riverstyx.o \
    rooms/hadesthrone.o \
    rooms/credits.o \
    rooms/intro.o \
    rooms/ferry.o \
    rooms/options.o \
    rooms/monster.o \
    rooms/monster/projectile.o \
    rooms/monster/typhoon.o \
    rooms/monster/cyclops.o \
    rooms/monster/illusion.o \
    rooms/medusa.o \
    rooms/trojan.o \
    gfx_context.o \
    ambient.o \
    herobelt.o \
    hotzone.o \
    table.o \
    persistent.o

DETECT_OBJS += $(MODULE)/detection.o

# This module can be built as a plugin
ifeq ($(ENABLE_HADESCH), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

POTFILE := $(srcdir)/engines/hadesch/po/hadesch.pot
POFILES := $(wildcard $(srcdir)/engines/hadesch/po/*.po)

hadesch-updatepot:
	cat $(srcdir)/engines/hadesch/po/POTFILES_hadesch | \
	xgettext -f - -D $(srcdir) -d hadesch --c++ -k_ -k_s -k_c:1,2c -k_sc:1,2c -kTranscribedSound:2 --add-comments=I18N\
		-kDECLARE_TRANSLATION_ADDITIONAL_CONTEXT:1,2c -o $(POTFILE) \
		--copyright-holder="ScummVM Team" --package-name=ScummVM \
		--package-version=$(VERSION) --msgid-bugs-address=scummvm-devel@lists.scummvm.org -o $(POTFILE)_

	sed -e 's/SOME DESCRIPTIVE TITLE/LANGUAGE translation for ScummVM/' \
		-e 's/UTF-8/CHARSET/' -e 's/PACKAGE/ScummVM/' $(POTFILE)_ > $(POTFILE).new

	rm $(POTFILE)_
	if test -f $(POTFILE); then \
		sed -f $(srcdir)/po/remove-potcdate.sed < $(POTFILE) > $(POTFILE).1 && \
		sed -f $(srcdir)/po/remove-potcdate.sed < $(POTFILE).new > $(POTFILE).2 && \
		if cmp $(POTFILE).1 $(POTFILE).2 >/dev/null 2>&1; then \
			rm -f $(POTFILE).new; \
		else \
			rm -f $(POTFILE) && \
			mv -f $(POTFILE).new $(POTFILE); \
		fi; \
		rm -f $(POTFILE).1 $(POTFILE).2; \
	else \
		mv -f $(POTFILE).new $(POTFILE); \
	fi;

%.po: $(POTFILE)
	msgmerge $@ $(POTFILE) -o $@.new
	if cmp $@ $@.new >/dev/null 2>&1; then \
		rm -f $@.new; \
	else \
		mv -f $@.new $@; \
	fi;

hadesch-translations-dat: devtools/create_translations
	devtools/create_translations/create_translations $(POFILES)
	mv translations.dat $(srcdir)/gui/themes/hadesch_translations.dat

update-hadesch-translations: hadesch-updatepot $(POFILES) hadesch-translations-dat

update-hadesch-translations: hadesch-updatepot $(POFILES)
	@$(foreach file, $(POFILES), echo -n $(notdir $(basename $(file)))": ";msgfmt --statistic $(file);)
	@rm -f messages.mo

.PHONY: updatehadeschpot hadesch-translations-dat update-hadesch-translations
