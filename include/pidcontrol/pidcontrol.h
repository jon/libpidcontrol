
#ifdef __cplusplus
extern "C" {
#endif

struct pid_controller {
    double k_p; //< Proportional control gain
    double k_d; //< Differential control gain
    double k_i; //< Integral control gain

    double dt; //< Timestep

    double setpoint; //< Control setpoint
    double pv; //< Process value

    double ev, ev_prior; //< Error value and prior error value
    double de; //< Linear approximation of first derivative of error
    double ie; //< Right triangle approximation of integral of error

    double control_command; //< Command to be issued
    double command_clamp_min; //< Minimum control command value
    double command_clamp_max; //< Maximum control command value
};
typedef struct pid_controller pid_controller_t;

/**
 * Convenience method for setting up a pid_controller_t to ensure code and
 * binaries remain compatible subject to structure reorganization or
 * element renaming
 *
 * @function pid_init
 * @param controller The controller to initialize
 * @param k_p Proportional control gain
 * @param k_d Differential control gain
 * @param k_i Integral control gain
 * @param dt Discrete timestep
 * @param min_command Minimum control command. Pass zero for both clamp values to disable clamping
 * @param max_command Maximum control command
 */
void pid_init(pid_controller_t *controller, double k_p, double k_d, double k_i, double dt, double min_command, double max_command);

/**
 * Update a controller's error values (true, derivative, and integral) with
 * a sample from reality
 *
 * @function pid_update_error
 * @param controller The controller instance to update
 * @param process_value The latest sampled process value
 */
void pid_update_error(pid_controller_t *controller, double process_value);

/**
 * Use the controller's current error values, gains, and clamps to calculate a
 * new control command
 *
 * @function pid_update_command
 * @param controller Controller to update
 * @return The new control command value
 */
double pid_update_command(pid_controller_t *controller); 

/**
 * Convenience method to update a controller's error and command value in
 * one step. In general it's easier to use this than pid_update_error and
 * pid_update_command unless you need to update error values from some
 * external source (i.e., a Kalman filter) with more precise error data.
 *
 * Note that this does not actually apply the control command, thus allowing
 * you to feed forward additional control values
 *
 * @function pid_control
 * @param controller Controller to update
 * @param process_value The latest sampled process value
 * @return The new control command value
 */
double pid_control(pid_controller_t *controller, double process_value);

#ifdef __cplusplus
}
#endif
