/*
 * Copyright (C) 2000 by Jörgen Pehrson <jp@spektr.eu.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 USA.
 *
 $Id$
 */

#ifndef _battstat_h_
#define _battstat_h_

#include <gio/gio.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <mate-panel-applet-gsettings.h>
#include <mate-panel-applet.h>

#define DEBUG 0

#define PROGLEN 33.0

/* I made these multipliers up
 *  --davyd
 */
#define ORANGE_MULTIPLIER 1.5
#define YELLOW_MULTIPLIER 2.5

typedef enum {
  APPLET_SHOW_NONE = 0,
  APPLET_SHOW_PERCENT,
  APPLET_SHOW_TIME
} AppletTextType;

typedef enum {
  STATUS_PIXMAP_BATTERY,
  STATUS_PIXMAP_METER,
  STATUS_PIXMAP_AC,
  STATUS_PIXMAP_CHARGE,
  STATUS_PIXMAP_WARNING,
  STATUS_PIXMAP_NUM
} StatusPixmapIndex;

typedef enum {
  POWER_STATUS_OFF = 0,
  POWER_STATUS_ON,
  POWER_STATUS_UNKNOWN
} PowerStatus;

typedef struct {
  PowerStatus on_ac_power;
  PowerStatus charging;
  gboolean present;
  gint minutes;
  gint percent;
} BatteryStatus;

typedef enum {
  LAYOUT_NONE,
  LAYOUT_LONG,
  LAYOUT_TOPLEFT,
  LAYOUT_TOP,
  LAYOUT_LEFT,
  LAYOUT_CENTRE,
  LAYOUT_RIGHT,
  LAYOUT_BOTTOM
} LayoutLocation;

typedef struct {
  LayoutLocation status;
  LayoutLocation text;
  LayoutLocation battery;
} LayoutConfiguration;

typedef struct _ProgressData {
  GtkWidget *applet;

  /* flags set from gsettings or the properties dialog */
  GSettings *settings;
  guint red_val;
  guint orange_val;
  guint yellow_val;
  gboolean red_value_is_time;
  gboolean lowbattnotification;
  gboolean fullbattnot;
  gboolean beep;
  AppletTextType showtext;

  /* label changed type (% <-> h:mm) and must be refreshed */
  gboolean refresh_label;

  /* the main grid that contains the visual elements */
  GtkWidget *grid;

  /* the visual elements */
  GtkWidget *battery;
  GtkWidget *status;
  GtkWidget *percent;

  /* dialog boxes that might be displayed */
  GtkWidget *prop_win;
  GtkWidget *battery_low_dialog;

  /* text label inside the low battery dialog */
  GtkLabel *battery_low_label;

  /* our height/width as given to us by size_allocate */
  gint width, height;

  /* should the battery meter be drawn horizontally? */
  gboolean horizont;

  /* on a vertical or horizontal panel? (up/down/left/right) */
  MatePanelAppletOrient orienttype;

  /* the current layout of the visual elements inside the grid */
  LayoutConfiguration layout;

  /* g_timeout source identifier */
  int timeout_id;
  int timeout;

  /* last_* for the benefit of the check_for_updates function */
  guint last_batt_life;
  StatusPixmapIndex last_pixmap_index;
  PowerStatus last_acline_status;
  PowerStatus last_charging;
  gboolean last_present;
  guint last_minutes;
} ProgressData;

/* battstat_applet.c */
void reconfigure_layout(ProgressData *battstat);
void battstat_show_help(ProgressData *battstat, const char *section);
void prop_cb(GtkAction *, ProgressData *);

/* power-management.c */
const char *power_management_getinfo(BatteryStatus *status);
const char *power_management_initialise(void (*callback)(void));
void power_management_cleanup(void);

gboolean power_management_using_upower(void);

#endif /* _battstat_h_ */
