#ifndef __MATEWEATHER_H__
#define __MATEWEATHER_H__

/*
 *  todd kulesza <fflewddur@dropline.net>
 *
 *  This code released under the GNU GPL.
 *  Read the file COPYING for more information.
 *
 * main header file
 *
 */
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <libmateweather/mateweather-prefs.h>
#include <mate-panel-applet.h>

/* Radar map on by default. */
#define RADARMAP

G_BEGIN_DECLS

typedef struct _MateWeatherApplet {
  MatePanelApplet* applet;
  WeatherInfo* mateweather_info;

  GSettings* settings;

  GtkWidget* container;
  GtkWidget* box;
  GtkWidget* label;
  GtkWidget* image;

  MatePanelAppletOrient orient;
  guint size;
  guint timeout_tag;
  guint suncalc_timeout_tag;

  /* preferences  */
  MateWeatherPrefs mateweather_pref;

  GtkWidget* pref_dialog;

  /* dialog stuff */
  GtkWidget* details_dialog;
} MateWeatherApplet;

G_END_DECLS

#endif
