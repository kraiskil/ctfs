# Build each main() into a separate library,
# the target platform build then selects
# which one to use.
add_library(main_croak_only OBJECT
	croak_only.cc)
target_link_libraries(main_croak_only libtreefrog)

add_library(main_treefrog OBJECT
	treefrog.cc)
target_link_libraries(main_treefrog libtreefrog)
