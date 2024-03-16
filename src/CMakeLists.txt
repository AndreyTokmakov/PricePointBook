cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

include_directories(../../json/include)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Utilities.cpp Utilities.h
        PricePointBook.cpp PricePointBook.h
        Types.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
)