foreach(language ${languages})
    add_custom_command(
        OUTPUT "${CMAKE_SOURCE_DIR}/docs/man/dist/${language}/scolorpicker.1.gz"
        DEPENDS "${CMAKE_SOURCE_DIR}/docs/man/${language}/scolorpicker.1"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/docs/man/dist/${language}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/docs/man/${language}/scolorpicker.1"
        "${CMAKE_SOURCE_DIR}/docs/man/dist/${language}/scolorpicker.1"
        COMMAND gzip "${CMAKE_SOURCE_DIR}/docs/man/dist/${language}/scolorpicker.1"
    )

    list(APPEND manpages "${CMAKE_SOURCE_DIR}/docs/man/dist/${language}/scolorpicker.1.gz")
endforeach()

add_custom_target(dist
    DEPENDS ${manpages}
    VERBATIM
)

install(FILES "${CMAKE_SOURCE_DIR}/docs/man/dist/en/scolorpicker.1.gz"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
)

install(FILES "${CMAKE_SOURCE_DIR}/docs/man/dist/pl/scolorpicker.1.gz"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/pl/man1"
)