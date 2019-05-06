doc_option = get_option('documentation')
doc_required = doc_option == 'true'
docdir = join_paths(get_option('datadir'), 'doc', 'libvdpau')

doxygen = find_program('doxygen', required : doc_required)
dot = find_program('dot', required : doc_required)

if doc_option != 'false' and doxygen.found() and dot.found()
    doxyfile = configure_file(
        input : 'Doxyfile.in',
        output : 'Doxyfile',
        configuration : cdata
    )

    html = custom_target('libvdpau-docs',
        command : [doxygen, doxyfile],
        output : 'html',
        install : true,
        install_dir : docdir,
    )
endif
