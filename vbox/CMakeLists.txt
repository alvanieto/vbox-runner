kcoreaddons_add_plugin(krunner_vbox SOURCES vbox.cpp INSTALL_NAMESPACE "kf5/krunner")
target_link_libraries(krunner_vbox KF5::Runner KF5::Plasma Qt5::Widgets Qt5::Xml
    KF5::I18n KF5::Service KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM)

install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/vbox-runner)
