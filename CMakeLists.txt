cmake_minimum_required(VERSION 3.5)
project(boost_fs_tutorials LANGUAGES CXX)
find_package(Boost REQUIRED COMPONENTS filesystem) #program_options)

add_executable(hw4 hw04.cpp)
target_link_libraries(hw4 Boost::filesystem)

#add_executable(hw4 hw04.cpp)
#target_link_libraries(hw4 Boost::program_options)
