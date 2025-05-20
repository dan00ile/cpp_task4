# 'Google_test' is the subproject name
project(Google_tests)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(
        ${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
)

# Находим Qt6
find_package(Qt6 COMPONENTS
  Core
  Gui
  Widgets
  REQUIRED)

# Добавляем исполняемый файл тестов
add_executable(Google_Tests_run
        DictionaryTest.cpp
        LoggerTest.cpp
        MockMainWindowTest.cpp
        ../dictionary.cpp
        ../logger.cpp
)

# Линкуем с gtest и библиотеками Qt
target_link_libraries(Google_Tests_run
        gtest
        gtest_main
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

# Автоматическое открытие исходников Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Копирование DLL-файлов Qt для тестов в Windows
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET Google_Tests_run POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:Google_Tests_run>/plugins/platforms/")
        add_custom_command(TARGET Google_Tests_run POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:Google_Tests_run>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets)
        add_custom_command(TARGET Google_Tests_run POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:Google_Tests_run>")
    endforeach(QT_LIB)
    
    # Копирование DLL из директории основного проекта
    add_custom_command(TARGET Google_Tests_run POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/plugins"
            "$<TARGET_FILE_DIR:Google_Tests_run>/plugins")
    
    # Копируем все DLL-файлы из корневой директории сборки
    file(GLOB DLL_FILES "${CMAKE_BINARY_DIR}/*.dll")
    foreach(DLL_FILE ${DLL_FILES})
        add_custom_command(TARGET Google_Tests_run POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${DLL_FILE}"
                "$<TARGET_FILE_DIR:Google_Tests_run>")
    endforeach()
endif()