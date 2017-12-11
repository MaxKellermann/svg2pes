project('svg2pes', 'cpp', version: '0.1',
  default_options: [
    'cpp_std=c++14'
  ],
)

compiler = meson.get_compiler('cpp')

# TODO: use get_option('b_ndebug'), but that doesn't work with Meson 0.37.1
if get_option('buildtype') == 'debug'
  debug = true
else
  debug = false
endif

common_flags = [
  '-D_GNU_SOURCE',
  '-Wall',
  '-Wextra',
  '-Wwrite-strings', '-Wcast-qual', '-Wcast-align', '-Wfloat-equal',
  '-Wshadow', '-Wpointer-arith', '-Wsign-compare',
  '-Wmissing-declarations', '-Wmissing-noreturn', '-Wmissing-format-attribute',
  '-Wredundant-decls', '-Wno-long-long', '-Wundef',
  '-Wunused',
  '-Wundef',
]

add_global_arguments(
  common_flags,
  '-Wno-non-virtual-dtor',
  language: 'cpp',
)

libexpat = dependency('expat')

inc = include_directories('src')

executable(
  'svg2pes',
  'src/Main.cxx',
  'src/ExpatParser.cxx',
  'src/ExpatUtil.cxx',
  'src/SvgParser.cxx',
  'src/SvgArc.cxx',
  'src/CssColor.cxx',
  'src/PesColor.cxx',
  'src/PesWriter.cxx',
  include_directories: inc,
  dependencies: [
    libexpat,
  ],
  install: true,
)

executable(
  'pesdump',
  'src/Dump.cxx',
  include_directories: inc,
  dependencies: [
  ],
  install: true,
)
