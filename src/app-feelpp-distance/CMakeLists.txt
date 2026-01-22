feelpp_add_application(distance SRCS distance.cpp DEFS FEELPP_DIM=3)
install(TARGETS feelpp_app_distance RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})