add_executable(guoke_demo_app demo-app/main.cpp)
guoke_configure_target(guoke_demo_app)

add_library(guoke_demo_dll SHARED demo-dll/dllmain.cpp)
guoke_configure_target(guoke_demo_dll)

# MinGW otherwise adds a "lib" prefix to the DLL filename.
set_target_properties(guoke_demo_dll PROPERTIES PREFIX "")
