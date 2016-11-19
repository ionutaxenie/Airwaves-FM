project(Airwaves-FM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Complex.cpp Complex.h defines.h main.cpp Signal.cpp Signal.h SocketManager.cpp SocketManager.h
        SoundPlayer.cpp SoundPlayer.h)
add_executable(airwaves_fm ${SOURCE_FILES})
target_link_libraries(airwaves_fm pthread pulse-simple)