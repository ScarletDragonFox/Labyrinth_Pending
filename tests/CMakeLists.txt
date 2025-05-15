cmake_minimum_required(VERSION 3.21...3.31)

enable_testing()

add_executable(test_eventManager eventmanagertestexample.cpp)
target_link_libraries(test_eventManager PRIVATE lp_source_library dependancies)

add_test(NAME test_eventManager COMMAND test_eventManager)

add_executable(test_ECS ecs_test.cpp)
target_link_libraries(test_ECS PRIVATE lp_source_library dependancies)

add_test(NAME EntityComponentSystem_test COMMAND test_ECS)