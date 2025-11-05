struct AliasEntries {
  GArray* aliasNames;
  GArray* aliases;
};

static void InsertAliases(GArray * aliasNames, GArray* aliases) {
  const char *home = g_get_home_dir ();
  GString * fileContent = g_string_new(NULL);
  char *aliasPath = g_strdup_printf ("%s/.bashrc.d/dev-louiscouture-path-alias.sh", home);
  for (guint i = 0; i<aliasNames->len; i++) {
    gchar* aliasName = g_array_index(aliasNames, gchar*, i);
        gchar* alias = g_array_index(aliases, gchar*, i);
    g_string_append(fileContent, g_strdup_printf("alias %s='%s'\n", aliasName, alias));
  }
  GError ** writeContentsError = NULL;
    gboolean done = g_file_set_contents (
      aliasPath,
      fileContent->str,
      fileContent->len,
      writeContentsError);

  g_free (aliasPath);
  g_string_free (fileContent, TRUE);
}
struct AliasEntries*
GetAliases(void) {
  FILE *fptr;
  FILE *folderPointer ;
  const char *home = g_get_home_dir ();
  char *path = g_strdup_printf ("%s/.bashrc.d/dev-louiscouture-path-alias.sh", home);
  fptr = fopen (path, "r");
  if (fptr == NULL)
    {
      printf ("Err no such file");
      char *folder = g_strdup_printf ("%s/.bashrc.d", home);
      folderPointer = fopen (folder, "r");
      if (folderPointer == NULL)
        {
          GError **error = NULL;
          GFile *gfile = g_file_new_for_path (folder);
          g_file_make_directory (gfile, NULL, error);
        } else {
          fclose(folderPointer);
        }
      GError **errorFile = NULL;
      GFile *file = g_file_new_for_path (path);
      g_file_create (file, G_FILE_CREATE_NONE, NULL, errorFile);
      fflush (stdout);
    } else {
      fclose(fptr);
    }
  g_free (path);
  GString *fileContent = readFile ("/.bashrc.d/dev-louiscouture-path-alias.sh");

    printf ("%s", fileContent->str);

    GArray *aliasNames = g_array_new (FALSE, FALSE, sizeof (gpointer));
    GArray *aliases = g_array_new (FALSE, FALSE, sizeof (gpointer));

    if (strcmp (fileContent->str, "") != 0) {
      gchar** aliasStatements = g_strsplit (fileContent->str, "\n", -1);
      for (int i = 0; aliasStatements[i+1] != NULL; i++) {
        gchar** byEquals = g_strsplit(aliasStatements[i], "=",2);
        gchar ** aliasAndName = g_strsplit(byEquals[0], " ",2);
        g_array_append_val(aliasNames, aliasAndName[1]);

        gchar** quotes = g_strsplit (byEquals[1], "'", 3);
        g_array_append_val(aliases, quotes[1]);
      }
    }
    struct AliasEntries *aliasEntries = g_malloc0(sizeof(struct AliasEntries));
  aliasEntries->aliasNames = aliasNames;
  aliasEntries->aliases = aliases;
  return aliasEntries;
}
