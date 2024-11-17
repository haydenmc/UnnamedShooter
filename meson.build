project('unnamed-shooter', 'cpp',
    version: '0.0.1',
    default_options: [
        'cpp_std=c++20',
        'warning_level=2',
        'werror=true',
    ])

# Subprojects
fmt_wrap = subproject('fmt',
    default_options: [
        'cpp_std=c++20',
        'werror=false',
        'warning_level=1',
    ])
spdlog_wrap = subproject('spdlog',
    default_options: [
        'cpp_std=c++20',
        'tests=disabled',
        'compile_library=true',
        'external_fmt=enabled',
        'std_format=disabled',
        'werror=false',
        'warning_level=1',
        'default_library=static',
    ])
sdl2_wrap = subproject('sdl2',
    default_options: [
        'cpp_std=c++20',
        'test=false',
        'werror=false',
        'warning_level=1',
    ])

# Source files
sources = [
    'src/Configuration.cpp',
    'src/Display.cpp',
    'src/Entrypoint.cpp',
    'src/Input.cpp',
    'src/Renderer.cpp',
    'src/Simulation.cpp',
]

# Dependencies
dependencies = [
    dependency('fmt'),
    dependency('spdlog'),
    dependency('sdl2'),
]

# Subsystem for Windows
# This determines whether the process starts with a console window ('console')
# and calls main(...), or starts without a console ('windows') and calls
# WinMain(...)
win_subsystem = 'windows'

# Debug options
if get_option('buildtype').startswith('debug')
    add_project_arguments('-DDEBUG', language: 'cpp')
    win_subsystem = 'console'
endif

executable(meson.project_name(),
    dependencies: dependencies,
    cpp_pch: 'src/pch.h',
    include_directories: [
        'src',
    ],
    sources: sources,
    win_subsystem: win_subsystem)