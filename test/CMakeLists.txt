add_executable(test
	main.cpp
)

target_link_libraries(test
	PRIVATE
	njson
)

set_target_properties(test
	PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
)
