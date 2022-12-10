add_executable(txbuild
    CException.h
    CMakefile.cpp
    CMakefile.h
    CMetaFile.cpp
    CMetaFile.h
    CPath.h
    CProject.cpp
    CProject.h
    main.cpp
    SBuildOptions.h
)

target_link_libraries(txbuild tinyxml)
target_include_directories(txbuild PRIVATE ../tinyxml)
