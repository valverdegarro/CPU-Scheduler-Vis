// Number of terms between each UI update
// Warning: low values may tank performance
#define UPDATE_INTERVAL 50

typedef void (*update_f_ptr)(int, long double, float);
typedef void (*yield_f_ptr)(void);

typedef struct {
    int task_id;
    int terms;
    update_f_ptr update_f; //Function used to update UI
    yield_f_ptr yield_f; // Function used to yield CPU
    float yield_percent; // Yield CPU per every yield_percent of terms calculated
} ptask_params; // -1.0 indicates non-expropiative mode is disabled

long double run_pi_task(ptask_params* params);