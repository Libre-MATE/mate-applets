@YELP_HELP_RULES@

HELP_ID = mate-geyes
HELP_FILES = index.docbook legal.xml

HELP_MEDIA = figures/geyes_applet.png

if USE_NLS
# Add linguas to be ignored, e.g. IGNORE_HELP_LINGUAS = ca de es fr
IGNORE_HELP_LINGUAS =
HELP_LINGUAS = $(if $(IGNORE_HELP_LINGUAS), \
	$(filter-out $(IGNORE_HELP_LINGUAS),$(subst /,,$(dir $(wildcard */*.po)))), \
	$(subst /,,$(dir $(wildcard */*.po))) )
else
HELP_LINGUAS =
endif

-include $(top_srcdir)/git.mk
