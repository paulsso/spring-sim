cmake_minimum_required(VERSION 3.23)
project(spring_sim C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

option(ENABLE_SANITIZERS "ASan + UBSan" OFF)
if(ENABLE_SANITIZERS)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_library(spring src/integrators.c)
target_include_directories(spring PUBLIC include)
target_link_libraries(spring PUBLIC m)

add_executable(spring_sim src/main.c)
target_link_libraries(spring_sim PRIVATE spring)

# enable_testing()
# find_package(cmocka REQUIRED)
# add_executable(test_convergence tests/test_convergence.c)
# target_link_libraries(test_convergence PRIVATE spring cmocka::cmocka)
# add_test(NAME convergence COMMAND test_convergence)
