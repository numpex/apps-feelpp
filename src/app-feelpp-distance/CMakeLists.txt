set(DIM 2 3)
foreach(dim ${DIM})
feelpp_add_application(distance-${dim}d SRCS distance.cpp DEFS FEELPP_DIM=${dim})
endforeach()