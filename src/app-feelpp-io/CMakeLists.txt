set(DIM 2 3)
set(ORDER 2 3 4)
foreach(dim ${DIM})
foreach(order ${ORDER})
    feelpp_add_application(io-${dim}dP${order} SRCS feelpp-io.cpp DEFS FEELPP_DIM=${dim} ORDER=${order})
endforeach()
endforeach()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/feelpp-io.sh ${CMAKE_CURRENT_BINARY_DIR}/feelpp-io.sh COPYONLY)