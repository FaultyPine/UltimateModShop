project('UltimateModShop', ['c', 'cpp'],
    version: '0.0.0',
    default_options: [ 'buildtype=debug', 'strip=false', 'b_ndebug=if-release', 'cpp_std=c++1z' ],
)

subdir('library')

#src_files = files(
#    'src/main.cpp',
#    'src/utils.cpp',
#    'src/curl.cpp',
#    'src/main_activity.cpp',

#    'src/components_tab.cpp',
#    'src/submission_node.cpp',
#)

# not supposed to do this... but eh. Gotta reconfigure meson whenever you add a new file
c = run_command('grabber.sh')
sources = c.stdout().strip().split('\n')

UltimateModShop = executable(
    'UltimateModShop',
    [ sources, borealis_files ],
    dependencies : borealis_dependencies,
    install: true,
    include_directories: [ borealis_include, include_directories('src')],
    cpp_args: [ '-IC', '-g', '-O2', '-DBRLS_RESOURCES="./resources/"', '-w' ] + borealis_cpp_args,
    link_args: [ '-lcurl', ]
)
