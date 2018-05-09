# CMake module to search for the libyaml library
# (library for parsing YAML files)
#
# If it's found it sets LIBYAML_FOUND to TRUE
# and following variables are set:
#    YAML_INCLUDE_DIR
#    LIBYAML_LIBRARY
#
# This file is part of https://github.com/zerotao/yaml, by Andrew Danforth,
# with some modifications

FIND_PATH(YAML_INCLUDE_DIR NAMES yaml.h)
FIND_LIBRARY(YAML_LIBRARIES NAMES yaml libyaml)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Yaml DEFAULT_MSG YAML_LIBRARIES YAML_INCLUDE_DIR)
MARK_AS_ADVANCED(YAML_INCLUDE_DIR YAML_LIBRARIES)
