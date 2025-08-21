feelpp_add_application( parallelSum SRCS parallelSum.cpp )
feelpp_add_application( testKokkos SRCS test_kokkos.cpp )
# target_link_libraries( testKokkos Kokkos::kokkos )