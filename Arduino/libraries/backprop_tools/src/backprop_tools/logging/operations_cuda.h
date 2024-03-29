#ifndef BACKPROP_TOOLS_UTILS_LOGGING_OPERATIONS_CUDA_H
#define BACKPROP_TOOLS_UTILS_LOGGING_OPERATIONS_CUDA_H


#include <backprop_tools/devices/cuda.h>

namespace backprop_tools{
    namespace logging{
        template <typename DEVICE, typename A>
        void text(DEVICE& device, devices::logging::CUDA* logger, const char * a, const char * b){
            std::cout << a << b << std::endl;
        }
        template <typename DEVICE, typename A>
        void text(DEVICE& device, devices::logging::CUDA* logger, const A a){
        }
        template <typename DEVICE, typename A, typename B>
        void text(DEVICE& device, devices::logging::CUDA* logger, const A a, const B b){
        }
        template <typename DEVICE, typename A, typename B, typename C, typename D>
        void text(DEVICE& device, devices::logging::CUDA* logger, const A a, const B b, const C c, const D d){
        }
        template <typename DEVICE>
        void add_scalar(DEVICE& device, devices::logging::CUDA* logger, const char* key, const float value, const typename devices::logging::CUDA::index_t cadence = 1){
            //noop
        }
    }
}
#endif
