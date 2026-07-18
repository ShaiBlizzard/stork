project('stork', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++26'])

qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core'])

m_enum_dep = dependency('magic_enum')

sources = [
'src/main.cpp',
'src/argParser.cpp',
'src/argHandler.cpp',
'src/actWorker.cpp',
'src/shared.h'
]
inc = include_directories('src')

stork = executable('stork',
                   sources,
                   install : true,
                   include_directories : inc,
                   dependencies: [qt6_dep, m_enum_dep]
)

test('test', stork)