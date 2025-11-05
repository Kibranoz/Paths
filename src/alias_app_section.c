#include "alias_manager.c"

struct DeleteAliasInfos
{
  GtkWidget *elementToDelete;
  GtkWidget *parentElement;
};

struct AliasEntriesContext
{
  struct AliasEntries *aliasEntries;
  GtkWidget *parentElement;
};

static void
delete_alias (GtkButton *button, struct DeleteAliasInfos *deleteAliasInfos)
{
  gtk_box_remove (GTK_BOX (deleteAliasInfos->parentElement), deleteAliasInfos->elementToDelete);
  g_free (deleteAliasInfos);
}

GtkWidget *
generate_new_aliasBox (GtkWidget *parentElement)
{
  GtkWidget *aliasBox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *aliasName = gtk_entry_new ();
  GtkWidget *aliasEquals = gtk_label_new ("=");
  GtkWidget *alias = gtk_entry_new ();
  gtk_widget_set_hexpand (alias, TRUE);
  gtk_widget_set_hexpand (aliasName, TRUE);
  gtk_entry_set_placeholder_text (GTK_ENTRY (aliasName), "ALIAS_NAME");
  gtk_entry_set_placeholder_text (GTK_ENTRY (alias), "path/to/alias.sh");

  GtkWidget *deleteButton = gtk_button_new_from_icon_name ("user-trash-symbolic");

  gtk_box_append (GTK_BOX (aliasBox), aliasName);
  gtk_box_append (GTK_BOX (aliasBox), aliasEquals);
  gtk_box_append (GTK_BOX (aliasBox), alias);
  gtk_box_append (GTK_BOX (aliasBox), deleteButton);

  gtk_widget_set_margin_start (aliasBox, 10);
  gtk_widget_set_margin_top (aliasBox, 10);
  gtk_widget_set_margin_end (aliasBox, 10);

  gtk_box_append (GTK_BOX (parentElement), aliasBox);

  struct DeleteAliasInfos *infosToDelete = g_malloc0 (sizeof (struct DeleteAliasInfos));
  infosToDelete->parentElement = parentElement;
  infosToDelete->elementToDelete = aliasBox;

  g_signal_connect (deleteButton, "clicked", G_CALLBACK (delete_alias), infosToDelete);
  return aliasBox;
}

static void
applyAliases (GtkButton *button, GtkWidget *parentWidget)
{
  GArray *aliasNames = g_array_new (FALSE, FALSE, sizeof (gpointer));
  GArray *aliases = g_array_new (FALSE, FALSE, sizeof (gpointer));

  GtkWidget *aliasBox = gtk_widget_get_first_child (parentWidget);

  while (aliasBox != NULL)
    {
      GtkWidget *aliasNameEntryBox = gtk_widget_get_first_child (aliasBox);
      GtkEntryBuffer *aliasNameBuffer = gtk_entry_get_buffer (GTK_ENTRY (aliasNameEntryBox));
      const gchar *aliasNameText = gtk_entry_buffer_get_text (aliasNameBuffer);
      GtkWidget *equalSign = gtk_widget_get_next_sibling (aliasNameEntryBox);
      GtkWidget *aliasEntryBox = gtk_widget_get_next_sibling (equalSign);

      GtkEntryBuffer *aliasBuffer = gtk_entry_get_buffer (GTK_ENTRY (aliasEntryBox));
      const gchar *aliasText = gtk_entry_buffer_get_text (aliasBuffer);

      if (strcmp (aliasNameText, "") != 0 && strcmp (aliasText, "") != 0)
        {
          g_array_append_val (aliasNames, aliasNameText);
          g_array_append_val (aliases, aliasText);
        }
        aliasBox = gtk_widget_get_next_sibling (aliasBox);
    }
  InsertAliases (aliasNames, aliases);
  g_array_free (aliasNames, TRUE);
  g_array_free (aliases, TRUE);
}

static void
add_existing_aliases (struct AliasEntriesContext *aliasEntriesContext)
{
  if (aliasEntriesContext->aliasEntries->aliases->len > 0)
    {
      GtkWidget *parentElement = aliasEntriesContext->parentElement;
      for (guint i = 0; i < aliasEntriesContext->aliasEntries->aliases->len; i++)
        {
          gchar *alias = g_array_index (aliasEntriesContext->aliasEntries->aliases, gchar *, i);
          gchar *aliasName = g_array_index (aliasEntriesContext->aliasEntries->aliasNames, gchar *, i);

          if (strcmp (alias, "") != 0 || strcmp (aliasName, "") != 0)
            {
              GtkWidget *aliasBox = generate_new_aliasBox (aliasEntriesContext->parentElement);
              GtkWidget *aliasNameEntry = gtk_widget_get_first_child (aliasBox);
              GtkWidget *equalSign = gtk_widget_get_next_sibling (aliasNameEntry);
              GtkWidget *aliasEntry = gtk_widget_get_next_sibling (equalSign);
              GtkEntryBuffer *aliasNameBuffer = gtk_entry_get_buffer (GTK_ENTRY (aliasNameEntry));
              fflush (stdout);
              gssize lenAliasName = (gssize) strlen (aliasName);
              gtk_entry_buffer_set_text (aliasNameBuffer, aliasName, lenAliasName);
              GtkEntryBuffer *aliasBuffer = gtk_entry_get_buffer (GTK_ENTRY (aliasEntry));
              gssize lenAlias = (gssize) strlen (alias);
              gtk_entry_buffer_set_text (aliasBuffer, alias, lenAlias);
            }
        }
    }
}

static void
add_new_alias (GtkButton *button, GtkWidget *parentElement)
{
  generate_new_aliasBox (parentElement);
}

static void
show_alias_app_section (GtkBuilder *builder)
{
  GtkWidget *elem_boxes = GTK_WIDGET (gtk_builder_get_object (builder, "elem_boxes_aliases"));
  if (!elem_boxes)
    {
      g_printerr ("Missing element with id : elem_boxes_aliases");
      return;
    }

  struct AliasEntries *aliasEntries = GetAliases ();
  struct AliasEntriesContext aliasEntriesContext = { aliasEntries, elem_boxes };
  add_existing_aliases (&aliasEntriesContext);

  GtkWidget *newAliasButton = GTK_WIDGET (gtk_builder_get_object (builder, "add_new_alias"));

  GtkWidget *applyAliasesButton = GTK_WIDGET (gtk_builder_get_object (builder, "update_aliases"));

  g_signal_connect (newAliasButton, "clicked", G_CALLBACK (add_new_alias), elem_boxes);
  g_signal_connect (applyAliasesButton, "clicked", G_CALLBACK (applyAliases), elem_boxes);
}

