#include <glib/gi18n.h>
#include "AppState.h"


void app_state_element_add(AppState *appState) {
  g_array_append_val (appState->activeElements, appState->elementCount);
  appState->elementCount ++;
}

void app_state_element_remove(AppState *appState, int elementToRemove) {
  GArray *new_array = g_array_new (FALSE, FALSE, sizeof (int));
  guint n = appState->activeElements->len;
  int newIndex = 0;
  for (guint i = 0; i<= n; i++) {
    if (g_array_index(appState->activeElements,int,i) != elementToRemove) {
      g_array_insert_val(new_array, newIndex, g_array_index(appState->activeElements, int,i));
      newIndex++;
    }
  }
  GArray *oldArray = appState->activeElements;
   appState->activeElements = new_array;
  g_array_free(oldArray, TRUE);

}
