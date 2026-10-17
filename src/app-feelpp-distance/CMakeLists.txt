feelpp_add_application(distance SRCS distance.cpp DEFS FEELPP_DIM=3)
install(TARGETS feelpp_app_distance RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if ( FEELPP_PROJECT_ENABLE_TESTS )
  set(_distance_tests "${CMAKE_CURRENT_SOURCE_DIR}/../../tests")
  add_executable(test_distance_bvh_minimum "${_distance_tests}/distance_bvh_minimum.cpp")
  target_include_directories(test_distance_bvh_minimum PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  target_compile_features(test_distance_bvh_minimum PRIVATE cxx_std_17)
  add_test(NAME distance-bvh-minimum COMMAND test_distance_bvh_minimum)

  find_package(MPI REQUIRED COMPONENTS CXX)
  add_test(NAME distance-bvh-regression
    COMMAND "${Python_EXECUTABLE}" "${_distance_tests}/check_distance_bvh.py"
      --executable $<TARGET_FILE:feelpp_app_distance>
      --mesh "${_distance_tests}/unit_cube.geo"
      --mpiexec "${MPIEXEC_EXECUTABLE}"
      "--numproc-flag=${MPIEXEC_NUMPROC_FLAG}")
  set_tests_properties(distance-bvh-regression PROPERTIES TIMEOUT 300 PROCESSORS 2)
endif()
