#include <stdbool.h>


typedef struct {
    int execution;
    int period;
} task_config_t;


typedef struct gui_config {
    bool single_slide; // if true, generate everything in a single slide
    bool rm_enabled;
    bool edf_enabled;
    bool llf_enabled;
    task_config_t *task_config;
} gui_config;