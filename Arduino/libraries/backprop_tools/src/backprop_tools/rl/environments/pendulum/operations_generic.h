#ifndef BACKPROP_TOOLS_RL_ENVIRONMENTS_PENDULUM_OPERATIONS_GENERIC
#define BACKPROP_TOOLS_RL_ENVIRONMENTS_PENDULUM_OPERATIONS_GENERIC
#include "pendulum.h"
#include "../operations_generic.h"
namespace backprop_tools::rl::environments::pendulum {
    template <typename T>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT T clip(T x, T min, T max){
        x = x < min ? min : (x > max ? max : x);
        return x;
    }
    template <typename DEVICE, typename T>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT T f_mod_python(const DEVICE& dev, T a, T b){
        return a - b * math::floor(dev, a / b);
    }

    template <typename DEVICE, typename T>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT T angle_normalize(const DEVICE& dev, T x){
        return f_mod_python(dev, (x + math::PI<T>), (2 * math::PI<T>)) - math::PI<T>;
    }
}
namespace backprop_tools{
    template<typename DEVICE, typename SPEC, typename RNG>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT static void sample_initial_state(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, typename rl::environments::Pendulum<SPEC>::State& state, RNG& rng){
        state.theta     = random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), SPEC::PARAMETERS::initial_state_min_angle, SPEC::PARAMETERS::initial_state_max_angle, rng);
        state.theta_dot = random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), SPEC::PARAMETERS::initial_state_min_speed, SPEC::PARAMETERS::initial_state_max_speed, rng);
    }
    template<typename DEVICE, typename SPEC>
    static void initial_state(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, typename rl::environments::Pendulum<SPEC>::State& state){
        state.theta = -math::PI<typename SPEC::T>;
        state.theta_dot = 0;
    }
    template<typename DEVICE, typename SPEC, typename ACTION_SPEC>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT typename SPEC::T step(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, const typename rl::environments::Pendulum<SPEC>::State& state, const Matrix<ACTION_SPEC>& action, typename rl::environments::Pendulum<SPEC>::State& next_state) {
        static_assert(ACTION_SPEC::ROWS == 1);
        static_assert(ACTION_SPEC::COLS == 1);
        using namespace rl::environments::pendulum;
        typedef typename SPEC::T T;
        typedef typename SPEC::PARAMETERS PARAMS;
        T u_normalised = get(action, 0, 0);
        T u = PARAMS::max_torque * u_normalised;
        T g = PARAMS::g;
        T m = PARAMS::m;
        T l = PARAMS::l;
        T dt = PARAMS::dt;

        u = clip(u, -PARAMS::max_torque, PARAMS::max_torque);

        T newthdot = state.theta_dot + (3 * g / (2 * l) * math::sin(typename DEVICE::SPEC::MATH(), state.theta) + 3.0 / (m * l * l) * u) * dt;
        newthdot = clip(newthdot, -PARAMS::max_speed, PARAMS::max_speed);
        T newth = state.theta + newthdot * dt;

        next_state.theta = newth;
        next_state.theta_dot = newthdot;
        return SPEC::PARAMETERS::dt;
    }
    template<typename DEVICE, typename SPEC, typename ACTION_SPEC>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT static typename SPEC::T reward(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, const typename rl::environments::Pendulum<SPEC>::State& state, const Matrix<ACTION_SPEC>& action, const typename rl::environments::Pendulum<SPEC>::State& next_state){
        using namespace rl::environments::pendulum;
        typedef typename SPEC::T T;
        T angle_norm = angle_normalize(typename DEVICE::SPEC::MATH(), state.theta);
        T u_normalised = get(action, 0, 0);
        T u = SPEC::PARAMETERS::max_torque * u_normalised;
        T costs = angle_norm * angle_norm + 0.1 * state.theta_dot * state.theta_dot + 0.001 * (u * u);
        return -costs;
    }

    template<typename DEVICE, typename SPEC, typename OBS_SPEC>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT static void observe(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, const typename rl::environments::Pendulum<SPEC>::State& state, Matrix<OBS_SPEC>& observation){
        static_assert(OBS_SPEC::ROWS == 1);
        static_assert(OBS_SPEC::COLS == 3);
        typedef typename SPEC::T T;
        set(observation, 0, 0, math::cos(typename DEVICE::SPEC::MATH(), state.theta));
        set(observation, 0, 1, math::sin(typename DEVICE::SPEC::MATH(), state.theta));
        set(observation, 0, 2, state.theta_dot);
    }
    // get_serialized_state is not generally required, it is just used in the WASM demonstration of the project page, where serialization is needed to go from the WASM runtime to the JavaScript UI
    template<typename DEVICE, typename SPEC>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT static typename SPEC::T get_serialized_state(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, const typename rl::environments::Pendulum<SPEC>::State& state, typename DEVICE::index_t index){
        if(index == 0) {
            return state.theta;
        }
        else{
            return state.theta_dot;
        }
    }
    template<typename DEVICE, typename SPEC, typename RNG>
    BACKPROP_TOOLS_FUNCTION_PLACEMENT static bool terminated(DEVICE& device, const rl::environments::Pendulum<SPEC>& env, const typename rl::environments::Pendulum<SPEC>::State state, RNG& rng){
        using T = typename SPEC::T;
        return false; //random::uniform_real_distribution(typename DEVICE::SPEC::RANDOM(), (T)0, (T)1, rng) > 0.9;
    }
}
#endif
