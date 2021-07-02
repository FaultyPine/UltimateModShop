project('UltimateModShop', ['c', 'cpp'],
    version: '0.0.0',
    default_options: [ 'buildtype=debug', 'strip=false', 'b_ndebug=if-release', 'cpp_std=c++1z' ],
)

subdir('library')

thread_dep = dependency('threads')
zlib = dependency('zlib')

# a lil cursed lol... P.S. Gotta reconfigure meson whenever you add a new file (meson --reconfigure build)
c = run_command('grabber.sh')
sources = c.stdout().strip().split('\n')

UltimateModShop = executable(
    'UltimateModShop',
    [ sources, borealis_files ],
    dependencies : [borealis_dependencies, thread_dep, zlib],
    install: true,
    cpp_pch : 'src/ums_pch.h',
    include_directories: [ borealis_include, include_directories('src')],
    cpp_args: ['-g', '-O0', '-DBRLS_RESOURCES="./resources/"', '-w' ] + borealis_cpp_args, # switch to -03 for release
    link_args: ['-lcurl', '-larchive',]
)
