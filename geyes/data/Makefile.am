NULL =

appletdir       = $(datadir)/mate-panel/applets
applet_in_files = org.mate.applets.GeyesApplet.mate-panel-applet.desktop.in
applet_DATA     = $(applet_in_files:.mate-panel-applet.desktop.in=.mate-panel-applet)

$(applet_in_files): $(applet_in_files).in Makefile
	$(AM_V_GEN)$(SED) \
            -e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
            -e "s|\@VERSION\@|$(PACKAGE_VERSION)|" \
            $< > $@

$(applet_DATA): $(applet_in_files) Makefile
	$(AM_V_GEN)$(MSGFMT) --desktop --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

servicedir       = $(datadir)/dbus-1/services
service_in_files = org.mate.panel.applet.GeyesAppletFactory.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.mate.panel.applet.GeyesAppletFactory.service: $(service_in_files)
	$(AM_V_GEN)$(SED) \
            -e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
            $< > $@

geyes_gschema_in_files = org.mate.panel.applet.geyes.gschema.xml.in
gsettings_SCHEMAS = $(geyes_gschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

CLEANFILES = \
	$(applet_DATA)			\
	$(applet_in_files)		\
	$(service_DATA)			\
	$(gsettings_SCHEMAS)		\
	*.gschema.valid			\
	$(NULL)

EXTRA_DIST = \
	geyes-applet-menu.xml		\
	geyes-resources.gresource.xml	\
	themes.ui			\
	$(applet_in_files).in		\
	$(service_in_files)		\
	$(geyes_gschema_in_files)	\
	$(NULL)

-include $(top_srcdir)/git.mk
