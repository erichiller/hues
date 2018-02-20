#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
# see: http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html
# 

PROJECT_NAME := Colors_in_C

include $(IDF_PATH)/make/project.mk

GCC_COLORS := error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01
CC += -fdiagnostics-color=always
CXX += -fdiagnostics-color=always
CFLAGS += -fdiagnostics-color=always
CPPFLAGS += -fdiagnostics-color=always
CXXFLAGS += -fdiagnostics-color=always