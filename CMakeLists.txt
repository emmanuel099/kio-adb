project(kio-adb)

set(QT_MIN_VERSION "5.4.0")
set(KF5_MIN_VERSION "5.3.0")
set(CMAKE_MIN_VERSION "2.8.12")
set(ECM_MIN_VERSION "1.6.0")

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION} FATAL_ERROR)

# ECM
find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMMarkNonGuiExecutable)
include(ECMPackageConfigHelpers)

# Qt
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)

# KF5
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    KIO
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

################################################################################

add_definitions(
    -DTRANSLATION_DOMAIN=\"kio_adb\"
    -DQT_USE_FAST_CONCATENATION
    -DQT_USE_FAST_OPERATOR_PLUS
)

set (kio_adb_SRCS
    kio_adb.cpp
    kio_adb_debug.cpp
)

add_library(kio_adb MODULE ${kio_adb_SRCS})
target_link_libraries(kio_adb KF5::KIOCore)
set_target_properties(kio_adb PROPERTIES OUTPUT_NAME "adb")

install(TARGETS kio_adb DESTINATION ${PLUGIN_INSTALL_DIR}/kf5/kio)

install(FILES adb.protocol DESTINATION ${SERVICES_INSTALL_DIR})
