project('libvdpau', 'c', 'cpp', meson_version : '>=0.41')
cc = meson.get_compiler('c')

libdir = join_paths(get_option('prefix'), get_option('libdir'))
moduledir = get_option('moduledir')
if moduledir == ''
    moduledir = join_paths(libdir, 'vdpau')
endif
sysconfdir = join_paths(get_option('prefix'), get_option('sysconfdir'))

cdata = configuration_data()
cdata.set_quoted('VDPAU_MODULEDIR', moduledir)
cdata.set_quoted('VDPAU_SYSCONFDIR', sysconfdir)
cdata.set_quoted('VDPAU_DOC_PATH', meson.current_source_dir())
cdata.set_quoted('VDPAU_HEADER_PATH', join_paths(meson.source_root(), 'include'))

if cc.has_function('secure_getenv')
    cdata.set10('HAVE_SECURE_GETENV', true)
    cdata.set10('_GNU_SOURCE', true)
elif cc.has_function('__secure_getenv')
    cdata.set10('HAVE___SECURE_GETENV', true)
    cdata.set10('_GNU_SOURCE', true)
endif

dri2_option = get_option('dri2')
dri2_required = dri2_option == 'true'
dri2 = []
if dri2_option != 'false'
    dri2 += [
        dependency('dri2proto', version : '>= 2.2', required : dri2_required),
        dependency('xext', required : dri2_required),
    ]
endif

use_dri2 = dri2_option != 'false'
foreach dep : dri2
    use_dri2 = use_dri2 and dep.found()
endforeach
cdata.set10('DRI2', use_dri2)

configure_file(output : 'config.h', configuration : cdata)
add_project_arguments('-DHAVE_CONFIG_H', language : [ 'c', 'cpp' ])
inc = include_directories(['.', 'include'])

subdir('doc')
subdir('include')
subdir('src')
subdir('test')
subdir('trace')

import('pkgconfig').generate(
    name : 'VDPAU',
    filebase : 'vdpau',
    version : '1.3',
    description : 'The Video Decode and Presentation API for UNIX',
    libraries : '-L${libdir} -lvdpau',
    variables : 'moduledir=' + moduledir,
)
