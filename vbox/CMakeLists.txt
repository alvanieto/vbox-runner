set(krunner_vbox_SRCS
    vbox.cpp
)

add_library(krunner_vbox MODULE ${krunner_vbox_SRCS})
target_link_libraries(krunner_vbox KF5::Runner KF5::Plasma Qt5::Widgets Qt5::Xml
    KF5::I18n KF5::Service KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

install(TARGETS krunner_vbox DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-vbox.desktop DESTINATION ${SERVICES_INSTALL_DIR})

install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/vbox-runner)
