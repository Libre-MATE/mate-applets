/*
 * MATE CPUFreq Applet
 * Copyright (C) 2006 Carlos Garcia Campos <carlosgc@gnome.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * Authors : Carlos Garc�a Campos <carlosgc@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "cpufreq-utils.h"

#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_POLKIT
#include <gio/gio.h>
#endif /* HAVE_POLKIT */

guint cpufreq_utils_get_n_cpus(void) {
  gint mcpu = -1;
  gchar *file = NULL;
  static guint n_cpus = 0;

  if (n_cpus > 0) return n_cpus;

  do {
    if (file) g_free(file);
    mcpu++;
    file = g_strdup_printf("/sys/devices/system/cpu/cpu%d", mcpu);
  } while (g_file_test(file, G_FILE_TEST_EXISTS));
  g_free(file);

  if (mcpu >= 0) {
    n_cpus = (guint)mcpu;
    return mcpu;
  }

  mcpu = -1;
  file = NULL;
  do {
    if (file) g_free(file);
    mcpu++;
    file = g_strdup_printf("/proc/sys/cpu/%d", mcpu);
  } while (g_file_test(file, G_FILE_TEST_EXISTS));
  g_free(file);

  if (mcpu >= 0) {
    n_cpus = (guint)mcpu;
    return mcpu;
  }

  n_cpus = 1;

  return 1;
}

void cpufreq_utils_display_error(const gchar *message, const gchar *secondary) {
  GtkWidget *dialog;

  g_return_if_fail(message != NULL);

  dialog =
      gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                             GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
  if (secondary) {
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s",
                                             secondary);
  }

  gtk_window_set_title(GTK_WINDOW(dialog), ""); /* as per HIG */
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dialog), TRUE);
  g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
  gtk_widget_show(dialog);
}

#ifdef HAVE_POLKIT
#define CACHE_VALIDITY_SEC 2

static gboolean selector_is_available(void) {
  GDBusProxy *proxy;
  static GDBusConnection *system_bus = NULL;
  GError *error = NULL;
  GVariant *reply;
  gboolean result;

  if (!system_bus) {
    system_bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if (!system_bus) {
      g_warning("%s", error->message);
      g_error_free(error);

      return FALSE;
    }
  }

  proxy = g_dbus_proxy_new_sync(system_bus, G_DBUS_PROXY_FLAGS_NONE, NULL,
                                "org.mate.CPUFreqSelector",
                                "/org/mate/cpufreq_selector/selector",
                                "org.mate.CPUFreqSelector", NULL, &error);

  if (!proxy) {
    g_warning("%s", error->message);
    g_error_free(error);

    return FALSE;
  }

  reply = g_dbus_proxy_call_sync(proxy, "CanSet", NULL, G_DBUS_CALL_FLAGS_NONE,
                                 -1, NULL, &error);

  if (!reply) {
    g_warning("Error calling org.mate.CPUFreqSelector.CanSet: %s",
              error->message);
    g_error_free(error);
    result = FALSE;
  } else {
    g_variant_get(reply, "(b)", &result);
    g_variant_unref(reply);
  }

  g_object_unref(proxy);

  return result;
}

gboolean cpufreq_utils_selector_is_available(void) {
  static gboolean cache = FALSE;
  static time_t last_refreshed = 0;
  time_t now;

  time(&now);
  if (ABS(now - last_refreshed) > CACHE_VALIDITY_SEC) {
    cache = selector_is_available();
    last_refreshed = now;
  }

  return cache;
}
#else  /* !HAVE_POLKIT */
gboolean cpufreq_utils_selector_is_available(void) {
  struct stat *info;
  gchar *path = NULL;

  path = g_find_program_in_path("cpufreq-selector");
  if (!path) return FALSE;

  if (geteuid() == 0) {
    g_free(path);
    return TRUE;
  }

  info = (struct stat *)g_malloc(sizeof(struct stat));

  if ((lstat(path, info)) != -1) {
    if ((info->st_mode & S_ISUID) && (info->st_uid == 0)) {
      g_free(info);
      g_free(path);

      return TRUE;
    }
  }

  g_free(info);
  g_free(path);

  return FALSE;
}
#endif /* HAVE_POLKIT_MATE */

gchar *cpufreq_utils_get_frequency_label(guint freq) {
  gint divisor;

  if (freq > 999999) /* freq (kHz) */
    divisor = (1000 * 1000);
  else
    divisor = 1000;

  if (((freq % divisor) == 0) || divisor == 1000) /* integer */
    return g_strdup_printf("%d", freq / divisor);
  else /* float */
    return g_strdup_printf("%3.2f", ((gfloat)freq / divisor));
}

gchar *cpufreq_utils_get_frequency_unit(guint freq) {
  if (freq > 999999) /* freq (kHz) */
    return g_strdup("GHz");
  else
    return g_strdup("MHz");
}

gboolean cpufreq_utils_governor_is_automatic(const gchar *governor) {
  g_return_val_if_fail(governor != NULL, FALSE);

  if (g_ascii_strcasecmp(governor, "userspace") == 0) return FALSE;

  return TRUE;
}

gboolean cpufreq_file_get_contents(const gchar *filename, gchar **contents,
                                   gsize *length, GError **error) {
  gint fd;
  GString *buffer = NULL;
  gchar *display_filename;

  g_return_val_if_fail(filename != NULL, FALSE);
  g_return_val_if_fail(contents != NULL, FALSE);

  display_filename = g_filename_display_name(filename);

  *contents = NULL;
  if (length) *length = 0;

  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    gint save_errno = errno;

    g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(save_errno),
                "Failed to open file '%s': %s", display_filename,
                g_strerror(save_errno));
    g_free(display_filename);

    return FALSE;
  }

  while (TRUE) {
    ssize_t bytes_read;
    gchar buf[1024];

    bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read < 0) { /* Error */
      if (errno != EINTR) {
        gint save_errno = errno;

        g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(save_errno),
                    "Failed to read from file '%s': %s", display_filename,
                    g_strerror(save_errno));

        if (buffer) g_string_free(buffer, TRUE);

        g_free(display_filename);
        close(fd);

        return FALSE;
      }
    } else if (bytes_read == 0) { /* EOF */
      break;
    } else {
      if (!buffer) buffer = g_string_sized_new(bytes_read);
      buffer = g_string_append_len(buffer, buf, bytes_read);
    }
  }

  g_free(display_filename);

  if (buffer) {
    *contents = g_string_free(buffer, FALSE);

    if (length) *length = strlen(*contents);
  }

  close(fd);

  return TRUE;
}
