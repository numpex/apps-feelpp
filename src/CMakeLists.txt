add_compile_options(-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3)
feelpp_add_testcase(cases PREFIX fp CATEGORY feelpp)
add_subdirectory(app-feelpp-io)
add_subdirectory(app-feelpp-distance)
