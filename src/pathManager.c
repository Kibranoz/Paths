#include <glib/gi18n.h>
static GArray *
GetPaths (void)
{
  FILE *fptr;
  FILE *folderPointer;
  const char *home = g_get_home_dir ();
  char *path = g_strdup_printf ("%s/.bashrc.d/dev-louiscouture-path.sh", home);
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
        }
      GError **errorFile = NULL;
      GFile *file = g_file_new_for_path (path);
      g_file_create (file, G_FILE_CREATE_NONE, NULL, errorFile);
      fptr = fopen (path, "r");
      fflush (stdout);
    }
  g_free (path);
  GString *fileContent;
  fileContent = g_string_new (NULL);
  int c;
  c = fgetc (fptr);

  while (c != EOF)
    {
      g_string_append_c (fileContent, c);
      c = fgetc (fptr);
    }
  printf ("%s", fileContent->str);
  printf ("%d", (int) strlen (fileContent->str));

  GArray *paths = g_array_new (FALSE, FALSE, sizeof (gpointer));

  if (strcmp (fileContent->str, "") != 0)
    {

      gchar **split1 = g_strsplit (fileContent->str, "=", 2);
      gchar **split2 = g_strsplit (split1[1], "$", 2);
      gchar **split3 = g_strsplit (split2[0], "\"", 2);
      gchar **split4 = g_strsplit (split3[1], ":", -1);

      for (guint i = 0; split4[i] != NULL; i++)
        {
          g_array_append_val (paths, split4[i]);
        }
    }

  return paths;
}

static void
InsertPaths (GArray *paths)
{
  FILE *fptr;
  const char *home = g_get_home_dir ();
  char *pathPath = g_strdup_printf ("%s/.bashrc.d/dev-louiscouture-path.sh", home);

  GString *pathString = g_string_new (NULL);

  for (guint i = 0; i < paths->len; i++)
    {
      gchar *element = g_array_index (paths, gpointer, i);
      if (strcmp (element, "") != 0)
        {
          g_string_append (pathString, g_array_index (paths, gpointer, i)); // will this work?
          g_string_append (pathString, ":");
        }
    }
  GString *pathFileContent = g_string_new (NULL);
  g_string_append (pathFileContent, "PATH=\"");
  g_string_append (pathFileContent, pathString->str);
  g_string_append (pathFileContent, "$PATH\"");

  printf ("%s\n", pathFileContent->str);
  GError **err;
  gboolean done = g_file_set_contents (
      pathPath,
      pathFileContent->str,
      pathFileContent->len,
      NULL);

  g_free (pathPath);
  g_string_free (pathFileContent, TRUE);
  g_string_free (pathString, TRUE);
}

