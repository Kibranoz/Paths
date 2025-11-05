#include <glib/gi18n.h>

static GString *
readFile (char *relPath)
{
  const char *home = g_get_home_dir ();
  char *path = g_strdup_printf ("%s/%s", home, relPath);

  GFile *file = g_file_new_for_path (path);

  GError **inputStreamError = NULL;
  GError **fileInfoError = NULL;
  GFileInputStream *fileInputStream = g_file_read (file, NULL, inputStreamError);

  GFileInfo *fileInfo = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_SIZE, G_FILE_QUERY_INFO_NONE, NULL, fileInfoError);

  guint64 fileSize = g_file_info_get_attribute_uint64 (fileInfo, G_FILE_ATTRIBUTE_STANDARD_SIZE);

  guint8 buffer[fileSize];
  gsize count = fileSize;
  gsize *bytes_read;
  GError **readFileError = NULL;

  GString *file_content = g_string_new (NULL);

  gsize read = g_input_stream_read (G_INPUT_STREAM (fileInputStream), buffer, count, NULL, readFileError);
  if (read > 0)
    {
      g_string_append (file_content, buffer);
    }
  if (read == -1)
    {
      printf ("%s", (*readFileError)->message);
    }

  GError **closeInputStream = NULL;

  g_object_unref(fileInfo);

  g_input_stream_close (G_INPUT_STREAM (fileInputStream), NULL, closeInputStream);
  g_object_unref (fileInputStream);
  return file_content;
}
