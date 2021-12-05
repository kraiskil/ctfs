# Build each main() into a separate library,
# the target platform build then selects
# which one to use.
add_library(main_croak_only OBJECT
	croak_only.cc)
target_link_libraries(main_croak_only libtreefrog)

add_library(main_treefrog OBJECT
	treefrog.cc)
target_link_libraries(main_treefrog libtreefrog)

add_library(main_blink_led OBJECT
	blink_led.cc)
target_link_libraries(main_blink_led libtreefrog)
