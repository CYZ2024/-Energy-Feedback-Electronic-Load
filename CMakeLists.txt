cmake_minimum_required(VERSION 3.20)

project(energy_feedback_load C)

option(BUILD_HOST_TESTS "Build host-side unit and integration tests" ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Warnings)

add_library(efload
    firmware/math/src/clarke_park.c
    firmware/math/src/lpf.c
    firmware/math/src/pi.c
    firmware/math/src/pll.c
    firmware/math/src/ramp.c
    firmware/math/src/svpwm.c
    firmware/control/src/cc_rectifier.c
    firmware/control/src/cvcf_source.c
    firmware/control/src/dq_current_loop.c
    firmware/app/src/converter_tasks.c
    firmware/app/src/startup_sequence.c
    firmware/app/src/supervisor.c
)

target_include_directories(efload
    PUBLIC
        firmware/core/include
        firmware/math/include
        firmware/control/include
        firmware/app/include
        firmware/hal/include
        firmware/config
)

efload_enable_warnings(efload)

if(NOT MSVC)
    target_link_libraries(efload PUBLIC m)
endif()

include(CTest)

if(BUILD_HOST_TESTS)
    add_library(efload_host_platform
        firmware/platform/host/src/hal_mock.c
        firmware/platform/host/src/plant_sim.c
    )
    target_include_directories(efload_host_platform
        PUBLIC
            firmware/platform/host/include
    )
    target_link_libraries(efload_host_platform PUBLIC efload)
    efload_enable_warnings(efload_host_platform)

    function(add_efload_test name source)
        add_executable(${name} ${source})
        target_link_libraries(${name} PRIVATE efload efload_host_platform)
        efload_enable_warnings(${name})
        add_test(NAME ${name} COMMAND ${name})
    endfunction()

    add_efload_test(test_clarke_park tests/unit/test_clarke_park.c)
    add_efload_test(test_pi tests/unit/test_pi.c)
    add_efload_test(test_pll tests/unit/test_pll.c)
    add_efload_test(test_svpwm tests/unit/test_svpwm.c)
    add_efload_test(test_cc_closed_loop tests/integration/test_cc_closed_loop.c)
    add_efload_test(test_cvcf_closed_loop tests/integration/test_cvcf_closed_loop.c)
    add_efload_test(test_fault_latch tests/safety/test_fault_latch.c)
endif()
