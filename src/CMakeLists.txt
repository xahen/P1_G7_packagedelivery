# Add package_distribution as a library
add_library(package_functions package_distribution.c delivery_algorithm.c delivery_sim.c resource_calculation.c create_routes.c astar_helper_functions.c clarke_and_wright_helper_functions.c)
target_include_directories(package_functions PUBLIC .)

# Add executable file
add_executable(run_program run_program.c)
target_link_libraries(run_program package_functions)