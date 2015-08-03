
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(Plugin)

include(BroPlugin)

#
# rabbitmq-c
#
set(Rabbitmqc_DIR ../../common/rabbitmq-c/install/)
find_package(Rabbitmqc REQUIRED)
set(LIBS ${LIBS} Rabbitmqc_LIBRARIES)
include_directories(BEFORE ${Rabbitmqc_INCLUDE_DIR})
message(STATUS "Found RabbitMQ library: ${Rabbitmqc_LIBRARIES}")

#
# amqpcpp
#
set(amqpcpp_DIR ../../common/amqpcpp/install/)
find_package(amqpcpp REQUIRED)
set(LIBS ${LIBS} amqpcpp_LIBRARIES)
include_directories(BEFORE ${amqpcpp_INCLUDE_DIR})
set(INCLUDES ${INCLUDES} ${amqpcpp_INCLUDE_DIR})
message(STATUS "Found RabbitMQ library: ${amqpcpp_LIBRARIES}")


if (amqpcpp_FOUND)
    bro_plugin_begin(PS amqp)
    bro_plugin_cc(src/Plugin.cc src/amqp.cc src/message_bus.cc)
    bro_plugin_bif(src/amqp.bif)
    bro_plugin_dist_files(README CHANGES COPYING VERSION)
    bro_plugin_link_library (${amqpcpp_LIBRARIES})
    bro_plugin_link_library (${Rabbitmqc_LIBRARIES})
    bro_plugin_end()
else ()
        message(FATAL_ERROR "amqpcpp not found.")
endif ()
