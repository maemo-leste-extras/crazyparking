SUBDIRS = mazes \
	  pixmaps \
	  sounds

bin_SCRIPTS = crazyparking.sh

dbusdir = $(datadir)/dbus-1/services
dbus_DATA = \
	    br.org.indt.crazyparking.service \
	    br.org.indt.crazyparking_startup.service


configdir = $(datadir)/crazyparking
config_DATA = crazyparking.conf

desktopdir = $(datadir)/applications/hildon
desktop_DATA = crazyparking.desktop

gamedir = $(wrapperdir)/games
game_DATA = crazyparking.game

EXTRA_DIST = \
	$(dbus_DATA) \
	$(config_DATA) \
	$(game_DATA) \
	$(desktop_DATA) \
	$(bin_SCRIPTS) \
	br.org.indt.crazyparking.service.in \
	br.org.indt.crazyparking_startup.service.in \
	crazyparking.conf.in \
	crazyparking.game.in \
	crazyparking.desktop.in \
	crazyparking.sh.in

edit = sed \
 	-e 's,@datadir\@,$(datadir),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@VERSION\@,$(VERSION),g'

crazyparking.sh: crazyparking.sh.in Makefile
	@$(edit) $< > $@

crazyparking.conf: crazyparking.conf.in Makefile
	@$(edit) $< > $@

crazyparking.desktop: crazyparking.desktop.in Makefile
	@$(edit) $< > $@

br.org.indt.crazyparking.service: br.org.indt.crazyparking.service.in Makefile
	@$(edit) $< > $@

br.org.indt.crazyparking_startup.service: br.org.indt.crazyparking_startup.service.in Makefile
	@$(edit) $< > $@

CLEANFILES = $(bin_SCRIPTS) $(config_DATA) $(desktop_DATA) $(dbus_DATA)
