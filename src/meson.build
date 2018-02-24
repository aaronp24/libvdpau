x11 = dependency('x11')
libdl = cc.find_library('dl', required : false)
threads =  dependency('threads')

vdpau_src = ['vdpau_wrapper.c']

if use_dri2
    vdpau_src += ['mesa_dri2.c']
endif

vdpau = shared_library('vdpau',
    sources : vdpau_src,
    include_directories : inc,
    dependencies : [x11, libdl, threads, dri2],
    version : '1.0.0',
    install : true,
)

install_data('vdpau_wrapper.cfg', install_dir : get_option('sysconfdir'))
