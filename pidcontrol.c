
#include <math.h>
#include <string.h>
#include <assert.h>
#include <pidcontrol/pidcontrol.h>

void pid_init(pid_controller_t *controller, double k_p, double k_d, double k_i, double dt, double min_command, double max_command) {
    memset(controller, 0, sizeof(pid_controller_t));

    assert(k_p != 0 || k_d != 0 || k_i != 0);
    assert(max_command > min_command || (max_command == 0 && min_command == 0));
    assert(dt > 0);

    controller->k_p = k_p;
    controller->k_d = k_d;
    controller->k_i = k_i;
    controller->dt = dt;
    controller->command_clamp_min = min_command;
    controller->command_clamp_max = max_command;

    controller->ev = nan("");
    controller->ev_prior = nan("");
}

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

void pid_update_error(pid_controller_t *controller, double process_value) {
    const double dt = controller->dt;

    controller->pv = process_value;

    controller->ev_prior = controller->ev;
    controller->ev = controller->setpoint - controller->pv;
    if (isnan(controller->ev_prior))
        controller->ev_prior = controller->ev; // First run
    controller->de = (controller->ev - controller->ev_prior) / controller->dt;

    double min_e = MIN(controller->ev_prior, controller->ev);
    double delta_e = fabs(controller->ev - controller->ev_prior);
    controller->ie = min_e*dt + delta_e*dt*0.5;
}

double pid_update_command(pid_controller_t *controller) {
    const double k_p = controller->k_p;
    const double k_d = controller->k_d;
    const double k_i = controller->k_i;

    controller->control_command = k_p*controller->ev + k_d*controller->de + k_i*controller->ie;
    
    if (controller->command_clamp_min != 0 || controller->command_clamp_max != 0) {
        controller->control_command = MAX(controller->control_command, controller->command_clamp_min);
        controller->control_command = MIN(controller->control_command, controller->command_clamp_max);
    }

    return controller->control_command;
}

double pid_control(pid_controller_t *controller, double process_value) {
    pid_update_error(controller, process_value);
    return pid_update_command(controller);
}

