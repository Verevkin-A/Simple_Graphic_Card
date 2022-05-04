# Install script for directory: /home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/libSDL2main.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/libSDL2.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2mainTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2mainTargets.cmake"
         "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/CMakeFiles/Export/lib64/cmake/SDL2/SDL2mainTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2mainTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2mainTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2" TYPE FILE FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/CMakeFiles/Export/lib64/cmake/SDL2/SDL2mainTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2" TYPE FILE FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/CMakeFiles/Export/lib64/cmake/SDL2/SDL2mainTargets-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2staticTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2staticTargets.cmake"
         "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/CMakeFiles/Export/lib64/cmake/SDL2/SDL2staticTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2staticTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2/SDL2staticTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2" TYPE FILE FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/CMakeFiles/Export/lib64/cmake/SDL2/SDL2staticTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2" TYPE FILE FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/CMakeFiles/Export/lib64/cmake/SDL2/SDL2staticTargets-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/cmake/SDL2" TYPE FILE FILES
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/SDL2Config.cmake"
    "/home/Verevkin/VSCode/izgProject/build/SDL2ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_assert.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_atomic.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_audio.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_bits.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_blendmode.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_clipboard.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_android.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_emscripten.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_iphoneos.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_macosx.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_minimal.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_os2.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_pandora.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_psp.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_windows.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_winrt.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_config_wiz.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_copying.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_cpuinfo.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_egl.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_endian.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_error.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_events.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_filesystem.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_gamecontroller.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_gesture.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_haptic.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_hidapi.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_hints.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_joystick.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_keyboard.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_keycode.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_loadso.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_locale.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_log.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_main.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_messagebox.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_metal.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_misc.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_mouse.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_mutex.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_name.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengl.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengl_glext.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengles.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengles2.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengles2_gl2.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengles2_gl2ext.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengles2_gl2platform.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_opengles2_khrplatform.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_pixels.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_platform.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_power.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_quit.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_rect.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_render.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_rwops.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_scancode.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_sensor.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_shape.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_stdinc.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_surface.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_system.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_syswm.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_assert.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_common.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_compare.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_crc32.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_font.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_fuzzer.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_harness.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_images.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_log.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_md5.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_memory.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_test_random.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_thread.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_timer.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_touch.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_types.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_version.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_video.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/SDL_vulkan.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/begin_code.h"
    "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/include/close_code.h"
    "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/include/SDL_config.h"
    "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/include/SDL_revision.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64/pkgconfig" TYPE FILE FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/sdl2.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/home/Verevkin/VSCode/izgProject/build/libs/SDL2-2.0.20/sdl2-config")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/aclocal" TYPE FILE FILES "/home/Verevkin/VSCode/izgProject/libs/SDL2-2.0.20/sdl2.m4")
endif()

