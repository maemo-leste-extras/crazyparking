project('crazyparking', 'c')

sdl = dependency('SDL')
sdl_mixer = dependency('SDL_mixer')

executable('crazyparking',
  'src/crazypark.c',
  'src/images.c',
  'src/level.c',
  install: true,
  dependencies: [sdl, sdl_mixer]
)

conf = configuration_data()
conf.set('DATADIR', join_paths(get_option('prefix'), 'share', 'crazyparking'))
conf.set('DESKTOPDIR', join_paths(get_option('prefix'), 'share', 'applications', 'hildon'))

configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: conf
)

install_data(sources: 'data/mazes/mazes.txt', install_dir: conf.get('DATADIR') + '/mazes')
install_data(sources: 'data/crazyparking.desktop', install_dir: conf.get('DESKTOPDIR'))
install_subdir('data/pixmaps', install_dir: conf.get('DATADIR'))
install_subdir('data/sounds', install_dir: conf.get('DATADIR'))
