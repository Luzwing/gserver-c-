#版本号要求
cmake_minimum_required (VERSION 3.0)


#项目
project (gameserver)

set (LIB_SRC
		base/Address.cc
		base/Cond.cc
		base/logger.cc
		base/Mutex.cc
		base/Socket.cc
		base/Thread.cc
		base/ThreadPool.cc
		base/util.cc
		reactor/EventLoop.cc
		reactor/Poller.cc
		reactor/Channel.cc
		reactor/Acceptor.cc
	)

add_library (cpan SHARED ${LIB_SRC})

find_library (PTHREAD pthread)

set (LIBS
		cpan
		pthread
	)

#add_subdirectory (base)

#aux_source_directory (. DIR_SRCS)

add_definitions(-std=c++11)
add_definitions(-g)

add_executable (endian "test/test_endian.cc" )
add_executable (eventloop1 "test/test_eventloop1.cc")
add_executable (eventloop2 "test/test_eventloop2.cc")
add_executable (test_acceptor "test/test_acceptor.cc")

#target_link_libraries (endian pthread)
#target_link_libraries (eventloop1 pthread)
target_link_libraries (eventloop1 ${LIBS})
target_link_libraries (eventloop2 ${LIBS})
target_link_libraries (test_acceptor ${LIBS})
