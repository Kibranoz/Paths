#include <glib/gi18n.h>
#include "fileManagement.c"
static void
checkBashRc (void)
{
  GString *bashRCContent = readFile (".bashrc");
  char *checkCodes[8];
  checkCodes[0] = "if [ -d ~/.bashrc.d ]; then";
  checkCodes[1] = "for rc in ~/.bashrc.d/*; do";
  checkCodes[2] = "if [ -f \"$rc\" ]; then";
  checkCodes[3] = ". \"$rc\"";
  checkCodes[4] = "fi";
  checkCodes[5] = "done";
  checkCodes[6] = "fi";
  checkCodes[7] = "unset rc";
  bool containsCode = true;
  for (int i = 0; i < 8; i++)
    {
      bool containsLine = strstr (bashRCContent->str, checkCodes[i]);
      if (!containsLine)
        {
          containsCode = false;
        }
    }
  if (!containsCode)
    {
      const char *home = g_get_home_dir ();
      char *path = g_strdup_printf ("%s/.bashrc", home);
      GFile *file = g_file_new_for_path (path);
      GError **fileOpenError = NULL;
      GFileOutputStream *ostream = g_file_append_to (file, G_FILE_CREATE_NONE, NULL, fileOpenError);

      gchar *toWrite = "if [ -d ~/.bashrc.d ]; then\n"
                      "    for rc in ~/.bashrc.d/*; do\n"
                      "        if [ -f \"$rc\" ]; then\n"
                      "            . \"$rc\"\n"
                      "        fi\n"
                      "    done\n"
                      "fi\n"
                      "unset rc\n";
      GError ** writeError = NULL;
      g_output_stream_write (G_OUTPUT_STREAM(ostream), toWrite, strlen(toWrite), NULL, writeError);
      GError ** closeError = NULL;

      g_output_stream_close (G_OUTPUT_STREAM (ostream), NULL, closeError);
      g_free(path);
      g_object_unref (file);
      g_object_unref (ostream);
    }
  g_string_free (bashRCContent, TRUE);
}

static GArray *
GetPaths (void)
{
  checkBashRc ();
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
  GString *fileContent = readFile ("/.bashrc.d/dev-louiscouture-path.sh");


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
  g_string_free (fileContent, TRUE);

  return paths;
}

static void
InsertPaths (GArray *paths)
{
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

