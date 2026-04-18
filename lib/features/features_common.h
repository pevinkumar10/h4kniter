#include "tool_state.h"

static const unsigned long NO_ACTION_SLEEP_MS = 30000UL;

extern bool is_sleep_eligible_screen(ToolScreen screen);
extern bool has_any_button_activity();
extern void enter_idle_sleep();
