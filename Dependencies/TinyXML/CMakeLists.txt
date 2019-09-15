cmake_minimum_required(VERSION 2.8.11)

project(TinyXML)

set(TINYXML_SOURCES
    tinyxml.cpp
    tinystr.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
)

set(TINYXML_HEADERS
    tinyxml.h
    tinystr.h
)

add_library(TinyXML STATIC ${TINYXML_SOURCES} ${TINYXML_HEADERS})

target_include_directories(TinyXML PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})