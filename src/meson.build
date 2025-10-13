paths3_sources = [
  'main.c',
  'paths3-application.c',
  'paths3-window.c',
]

paths3_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
]

paths3_sources += gnome.compile_resources('paths3-resources',
  'paths3.gresource.xml',
  c_name: 'paths3'
)

executable('paths3', paths3_sources,
  dependencies: paths3_deps,
       install: true,
)
