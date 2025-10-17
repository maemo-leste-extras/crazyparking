SUBDIRS = launcher plugin

bin_PROGRAMS = crazyparking

crazyparking_SOURCES = 	\
	callbacks.c 		\
	crazypark.c 		\
	images.c 		\
	level.c			\
	callbacks.h 		\
	images.h 		\
	level.h

crazyparking_CFLAGS = 							\
	-DHGW_FUNC=1							\
	$(SDL_CFLAGS) $(HILDON_CFLAGS) $(GTK_CFLAGS) $(GCONF_CFLAGS)	\
	-DSOUNDPREFIX=\"$(datadir)/crazyparking/sounds\" 		\
	-DGFXPREFIX=\"$(datadir)/crazyparking/pixmaps\"			\
	-DMAZEPREFIX=\"$(datadir)/crazyparking/mazes\"

crazyparking_LDADD = $(SDL_LIBS) $(SDL_MIXER_LIBS) $(HGW_LIBS) $(HILDON_LIBS) $(GTK_LIBS) $(GCONF_LIBS)
