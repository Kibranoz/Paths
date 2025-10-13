#ifndef APP_STATE_H
#define APP_STATE_H

typedef struct {
  int elementCount;
  GArray *activeElements;
} AppState;

void app_state_element_add(AppState *appState);
void app_state_element_remove(AppState *appState, int elementToRemove);

#endif
