#include "pathManager.c"

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
    }
  InsertPaths (paths);
  g_array_free (paths, TRUE);
}
static void
delete_path (GtkButton *button, struct DeletePathInfos *deletePathInfos)
{
  gtk_box_remove (GTK_BOX (deletePathInfos->parentElement), deletePathInfos->elementToDelete);
  g_free (deletePathInfos);
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
add_new_path (GtkButton *button, GtkWidget *parentElement)
{
  generate_new_pathBox (parentElement);
}
static void
add_existing_paths (struct ExistingPathEntries *existingPathEntries)
{
  if (existingPathEntries->paths->len > 0)
    {
      GtkWidget *parentElement = existingPathEntries->parentElement;
      for (guint i = 0; i < existingPathEntries->paths->len; i++)
        {
          gchar *text = g_array_index (existingPathEntries->paths, gchar *, i);
          if (strcmp (text, "") != 0)
            {
              GtkWidget *pathBox = generate_new_pathBox (parentElement);
              GtkWidget *entry_box = gtk_widget_get_first_child (pathBox);
              GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY (entry_box));
              gssize len = (gssize) strlen (text);
              gtk_entry_buffer_set_text (buffer, text, len);
            }
        }
    }
  g_array_free(existingPathEntries->paths, TRUE);
  g_free (existingPathEntries);
}

static void show_paths_app_section(GtkBuilder * builder) {
   GtkWidget *elem_boxes = GTK_WIDGET (gtk_builder_get_object (builder, "elem_boxes"));

  if (!elem_boxes)
    {
      g_printerr ("Missing element with id : elem_boxes_paths");
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
  g_signal_connect (newPathButton, "clicked", G_CALLBACK (add_new_path), elem_boxes);
  g_signal_connect (applyPathButton, "clicked", G_CALLBACK (apply_path_changes), elem_boxes);
}
