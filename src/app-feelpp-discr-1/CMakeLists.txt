
set(ORDERS 1 2 3)
set(DIM 2 3)
foreach(order ${ORDERS})
foreach(dim ${DIM})
        feelpp_add_application(discr_heat_${dim}DP${order} SRCS discr-heat.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_heat_lib DEFS ORDER=${order} FEELPP_DIM=${dim})
    endforeach()
endforeach()
