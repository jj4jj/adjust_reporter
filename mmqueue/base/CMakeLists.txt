
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} cur_aSRCS)
set(cur_SRCS "${cur_aSRCS}")

aux_source_directory(${PROJECT_SOURCE_DIR}/base cur_aSRCS)
set(cur_SRCS "${cur_SRCS};${cur_aSRCS}")
set(cur_SRCS "${cur_SRCS};${PROJECT_SOURCE_DIR}/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}
/usr/include
)

add_library(dcbase STATIC ${cur_SRCS})

link_directories(
/usr/local/lib)

target_link_libraries(dcbase
)

