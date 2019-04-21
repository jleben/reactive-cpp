
add_library(reactive STATIC
    signal.cpp
    reaction.cpp
)

target_link_libraries(reactive pthread)
