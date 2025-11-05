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

#include "paths_app_section.c"
#include "alias_app_section.c"

#include <stdlib.h>


struct _Paths3Application
{
  AdwApplication parent_instance;
};

struct MenuElements {
  GtkToggleButton * pathsButton;
  GtkToggleButton* aliasButton;
  GtkStack* stack;
  GtkBox * alias_box;
  GtkBox* path_box;
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

static void toggle_button_management(GtkToggleButton* selectedButton, struct MenuElements * elements)
{
  if (gtk_toggle_button_get_active(elements->aliasButton) && selectedButton == elements->aliasButton) {
    gtk_toggle_button_set_active (elements->pathsButton, false);
    gtk_stack_set_visible_child (elements->stack, GTK_WIDGET (elements->alias_box));
  }
  if (gtk_toggle_button_get_active(elements->pathsButton) && selectedButton == elements->pathsButton)   {
    gtk_toggle_button_set_active (elements->aliasButton, false);
    gtk_stack_set_visible_child (elements->stack, GTK_WIDGET (elements->path_box));
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


  GtkStack *stack = GTK_STACK(gtk_builder_get_object (builder, "main_stack"));
  GtkToggleButton * pathsButton = GTK_TOGGLE_BUTTON (gtk_builder_get_object(builder, "path_select_button"));
  GtkToggleButton * aliasButton = GTK_TOGGLE_BUTTON (gtk_builder_get_object(builder, "alias_select_button"));
  GtkBox * aliasBox = GTK_BOX (gtk_builder_get_object(builder, "main_box_aliases"));
  GtkBox * pathBox = GTK_BOX (gtk_builder_get_object(builder, "main-box"));

  struct MenuElements * menuElements = g_malloc0 (sizeof(struct MenuElements));
  menuElements->stack = stack;
  menuElements->pathsButton = pathsButton;
  menuElements->aliasButton = aliasButton;
  menuElements->path_box = pathBox;
  menuElements->alias_box = aliasBox;

  g_signal_connect (pathsButton, "toggled", G_CALLBACK (toggle_button_management), menuElements);
  g_signal_connect (aliasButton, "toggled", G_CALLBACK (toggle_button_management), menuElements);

  show_paths_app_section (builder);
  show_alias_app_section (builder);

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
