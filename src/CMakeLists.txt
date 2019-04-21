
add_library(reactive STATIC
    object.cpp
    signal.cpp
    reaction.cpp
)

target_link_libraries(reactive pthread)
