/* paths3-application.c
 *
 * Copyright 2025 Louis Couture
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"
#include <glib/gi18n.h>

#include "paths3-application.h"
#include "paths3-window.h"

#include <stdlib.h>

#include "AppState.c"
#include "pathManager.c"

struct _Paths3Application
{
  AdwApplication parent_instance;
};

struct PathEntry
{
  AppState *appState;
  GtkWidget *parentElement;
  int id;
};

struct ExistingPathEntries
{
  GArray *paths;
  GtkWidget *parentElement;
};

struct DeletePathInfos
{
  GtkWidget *elementToDelete;
  GtkWidget *parentElement;
};

G_DEFINE_FINAL_TYPE (Paths3Application, paths3_application, ADW_TYPE_APPLICATION)

Paths3Application *
paths3_application_new (const char *application_id,
                        GApplicationFlags flags)
{
  g_return_val_if_fail (application_id != NULL, NULL);

  return g_object_new (PATHS3_TYPE_APPLICATION,
                       "application-id", application_id,
                       "flags", flags,
                       "resource-base-path", "/dev/louiscouture/path",
                       NULL);
}
static void
apply_path_changes (GtkButton *button, GtkWidget *parentWidget)
{
  GArray *paths = g_array_new (FALSE, FALSE, sizeof (gpointer));
  GtkWidget *pathBox = gtk_widget_get_first_child (parentWidget);
  while (pathBox != NULL)
    {
      GtkWidget *entry_box = gtk_widget_get_first_child (pathBox);

      GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY (entry_box));
      const gchar *pathText = gtk_entry_buffer_get_text (buffer);
      g_array_append_val (paths, pathText);
      pathBox = gtk_widget_get_next_sibling (pathBox);
      printf ("%s\n", pathText);
  fflush (stdout);

    }
  InsertPaths (paths);
}
static void
delete_path (GtkButton *button, struct DeletePathInfos *deletePathInfos)
{
  gtk_box_remove (GTK_BOX (deletePathInfos->parentElement), deletePathInfos->elementToDelete);
  // to do : check if we need to keep id.
}

GtkWidget *
generate_new_pathBox (GtkWidget *parentElement)
{
  GtkWidget *pathBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *path = gtk_entry_new ();
  gtk_widget_set_hexpand (path, TRUE);
  gtk_entry_set_placeholder_text (GTK_ENTRY (path), "path/to/binary");

  GtkWidget *deleteButton = gtk_button_new_from_icon_name ("user-trash-symbolic");

  gtk_box_append (GTK_BOX (pathBox), path);
  gtk_box_append (GTK_BOX (pathBox), deleteButton);

  gtk_widget_set_margin_start (pathBox, 10);
  gtk_widget_set_margin_top (pathBox, 10);
  gtk_widget_set_margin_end (pathBox, 10);

  gtk_box_append (GTK_BOX (parentElement), pathBox);

  struct DeletePathInfos *infosToDelete = g_malloc0 (sizeof (struct DeletePathInfos));
  infosToDelete->parentElement = parentElement;
  infosToDelete->elementToDelete = pathBox;

  g_signal_connect (deleteButton, "clicked", G_CALLBACK (delete_path), infosToDelete);
  return pathBox;
}
static void
add_new_path (GtkButton *button, struct PathEntry *pathEntry)
{
  generate_new_pathBox (pathEntry->parentElement);
}
static void
add_existing_paths (struct ExistingPathEntries *existingPathEntries)
{
  if (existingPathEntries->paths->len > 0){
  GtkWidget *parentElement = existingPathEntries->parentElement;
  for (guint i = 0; i < existingPathEntries->paths->len; i++)
    {
      gchar *text = g_array_index (existingPathEntries->paths, gchar *, i);
      if (strcmp(text, "") != 0) {
      GtkWidget *pathBox = generate_new_pathBox (parentElement);
      GtkWidget *entry_box = gtk_widget_get_first_child (pathBox);
      GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY (entry_box));
      printf ("%s\n", text);
      fflush (stdout);
      gssize len = (gssize) strlen (text);
      gtk_entry_buffer_set_text (buffer, text, len);
      }
    }
  }
}
static void
paths3_application_activate (GApplication *app)
{
  GError *error = NULL;
  g_assert (PATHS3_IS_APPLICATION (app));

  //------------find critical elements-------------------------

  GResource *gres = g_resource_load ("paths3.gresource", &error);
  if (gres)
    {
      g_resources_register (gres);
      g_resource_unref (gres); /* still registered after unref */
    }
  else
    {
      if (error)
        {
          /* If you didn't build a runtime gresource this is OK — resources might be compiled-in. */
          g_clear_error (&error);
        }
    }

  GtkBuilder *builder = gtk_builder_new_from_resource ("/dev/louiscouture/path/paths3-window.ui");
  if (!builder)
    {
      g_printerr ("Failed to load UI resource\n");
      return;
    }
  GtkWidget *elem_boxes = GTK_WIDGET (gtk_builder_get_object (builder, "elem_boxes"));

  if (!elem_boxes)
    {
      g_printerr ("main_box not found in UI — check id (main_box vs main-box) and recompile resources\n");
      return;
    }
  GtkWidget *newPathButton = GTK_WIDGET (gtk_builder_get_object (builder, "add_new_path"));

  GtkWidget *applyPathButton = GTK_WIDGET (gtk_builder_get_object (builder, "update_paths"));

  //--------------Add existing elements -------------------------------------

  GArray *items = GetPaths ();

  struct ExistingPathEntries *existingPathEntries = g_malloc0 (sizeof (struct ExistingPathEntries));

  existingPathEntries->parentElement = elem_boxes;
  existingPathEntries->paths = items;
  add_existing_paths (existingPathEntries);

  // ---------create new path entry and connect it to add new path button-------

  AppState *appState = g_malloc0 (sizeof (AppState));
  appState->elementCount = 0;
  appState->activeElements = g_array_new (FALSE, FALSE, sizeof (int));

  struct PathEntry *pathEntry = g_malloc0 (sizeof (struct PathEntry));
  pathEntry->appState = appState;
  pathEntry->parentElement = elem_boxes;
  pathEntry->id = pathEntry->appState->elementCount;

  g_signal_connect (newPathButton, "clicked", G_CALLBACK (add_new_path), pathEntry);
  g_signal_connect (applyPathButton, "clicked", G_CALLBACK (apply_path_changes), elem_boxes);

  //---------------- Present app window ------------------------------------
  GtkWidget *window_ui = GTK_WIDGET (gtk_builder_get_object (builder, "Paths3Window"));

  gtk_window_set_application (GTK_WINDOW (window_ui), GTK_APPLICATION (app));

  gtk_window_present (GTK_WINDOW (window_ui));
}

static void
paths3_application_class_init (Paths3ApplicationClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  app_class->activate = paths3_application_activate;
}

static void
paths3_application_about_action (GSimpleAction *action,
                                 GVariant *parameter,
                                 gpointer user_data)
{
  static const char *developers[] = { "Louis Couture", NULL };
  Paths3Application *self = user_data;
  GtkWindow *window = NULL;

  g_assert (PATHS3_IS_APPLICATION (self));

  window = gtk_application_get_active_window (GTK_APPLICATION (self));

  adw_show_about_dialog (GTK_WIDGET (window),
                         "application-name", "paths",
                         "application-icon", "dev.louiscouture.path",
                         "developer-name", "Louis Couture",
                         "translator-credits", _ ("translator-credits"),
                         "version", "0.1.0",
                         "developers", developers,
                         "copyright", "© 2025 Louis Couture",
                         NULL);
}

static void
paths3_application_quit_action (GSimpleAction *action,
                                GVariant *parameter,
                                gpointer user_data)
{
  Paths3Application *self = user_data;

  g_assert (PATHS3_IS_APPLICATION (self));

  g_application_quit (G_APPLICATION (self));
}

static const GActionEntry app_actions[] = {
  { "quit", paths3_application_quit_action },
  { "about", paths3_application_about_action },
};

static void
paths3_application_init (Paths3Application *self)
{
  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   app_actions,
                                   G_N_ELEMENTS (app_actions),
                                   self);
  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "app.quit",
                                         (const char *[]) { "<primary>q", NULL });
}

