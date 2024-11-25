project('unnamed-shooter', 'cpp',
    version: '0.0.1',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
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
fpm_wrap = subproject('fpm',
    default_options: [
        'cpp_std=c++20',
        'werror=false',
        'warning_level=1',
    ])
catch2_wrap = subproject('catch2',
    default_options: [
        'cpp_std=c++20',
        'tests=false',
        'werror=false',
        'warning_level=1',
    ])

# Subsystem for Windows
# This determines whether the process starts with a console window ('console')
# and calls main(...), or starts without a console ('windows') and calls
# WinMain(...)
win_subsystem = 'windows'

# Debug options
if get_option('buildtype').startswith('debug')
    add_project_arguments('-DDEBUG', language: 'cpp')
    win_subsystem = 'console'
else
    add_project_arguments('-DNDEBUG', language: 'cpp')
endif

executable(meson.project_name(),
    cpp_pch: 'src/pch.h',
    dependencies: [
        dependency('fmt'),
        dependency('spdlog'),
        dependency('sdl2'),
        dependency('fpm'),
    ],
    include_directories: [
        'src',
    ],
    sources: [
        'src/Configuration.cpp',
        'src/Display.cpp',
        'src/Entrypoint.cpp',
        'src/Input.cpp',
        'src/Mesh.cpp',
        'src/Renderer/RenderTarget.cpp',
        'src/Renderer/Renderer.cpp',
        'src/Simulation.cpp',
        'src/Entities/CameraEntity.cpp',
    ],
    win_subsystem: win_subsystem)

tests_exe = executable(meson.project_name() + '.tests',
    cpp_pch: 'test/testpch.h',
    dependencies: [
        dependency('fmt'),
        dependency('spdlog'),
        dependency('sdl2'),
        dependency('fpm'),
        dependency('catch2-with-main'),
    ],
    include_directories: [
        'test',
        'src',
    ],
    sources: [
        # Test definitions
        'test/MathTests.cpp',
        'test/RenderTests.cpp',
        # Sources under test
        'src/Renderer/RenderTarget.cpp',
    ])

test('tests', tests_exe)