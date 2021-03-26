project('UltimateModShop', ['c', 'cpp'],
    version: '0.0.0',
    default_options: [ 'buildtype=debug', 'strip=false', 'b_ndebug=if-release', 'cpp_std=c++1z' ],
)

subdir('library')

src_files = files(
    'src/main.cpp',
    'src/utils.cpp',
    'src/curl.cpp',
    'src/main_activity.cpp'
)

UltimateModShop = executable(
    'UltimateModShop',
    [ src_files, borealis_files ],
    dependencies : borealis_dependencies,
    install: true,
    include_directories: [ borealis_include, include_directories('src')],
    cpp_args: [ '-g', '-O2', '-DBRLS_RESOURCES="./resources/"', '-w', ] + borealis_cpp_args,
    link_args: [ '-lcurl', ]
)
