include(ExternalProject)

ExternalProject_Add(
    glog-download
    PREFIX "glog"
    URL https://s3.amazonaws.com/fcs-build-public/glog-0.3.3.tar.gz
    URL_MD5 0766c420d0dcd16e0b46cd7cc8235a1f
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/glog/install"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")

ExternalProject_Add(
    googletest-download
    PREFIX "googletest"
    URL https://s3.amazonaws.com/fcs-build-public/googletest.tar.gz
    URL_MD5 18fda945045354e264e3cca5428525d6
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest/install"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")

ExternalProject_Add(
    falcon-lic-download
    PREFIX "falcon-lic"
    URL https://s3.amazonaws.com/fcs-build-public/falcon-lic.tar.gz
    URL_MD5 8f36e6cb08c7cb7a677d749bee1bfe6e
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/falcon-lic/install"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")

set(GLOG_DIR "${CMAKE_CURRENT_BINARY_DIR}/glog/install" CACHE INTERNAL "Path to glog dependencies.")
set(GTEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest/install" CACHE INTERNAL "Path to googletest dependencies.")
set(FLM_DIR "${CMAKE_CURRENT_BINARY_DIR}/falcon-lic/install" CACHE INTERNAL "Path to falcon-lic dependencies.")
