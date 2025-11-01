set(SRCS
    visible_craft.cc)

add_executable(visible_craft
    ${SRCS})
target_link_libraries(visible_craft
    sgp4)
