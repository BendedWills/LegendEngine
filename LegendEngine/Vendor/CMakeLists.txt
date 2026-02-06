cmake_minimum_required (VERSION 3.10)

include(FetchContent)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(DeclareGitRepo libName gitRepo)
    FetchContent_Declare(
        ${libName}
        GIT_REPOSITORY ${gitRepo}
        SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/${libName}
        BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-build
        SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-subbuild
    )
endfunction()

function(DeclareLibs)
    DeclareGitRepo(Tether https://github.com/BendedWills/Tether)
endfunction()

function(MakeLibsAvailable)
    if (NOT LGENG_TETHER_EXTERN)
        if (LGENG_VULKAN_API)
            option(TETHER_RENDERING_VULKAN_API ON)
            set(TETHER_RENDERING_VULKAN_API ON)
        endif()
        option(TETHER_BUILD_RENDERING ON)
        set(TETHER_BUILD_RENDERING ON)

        FetchContent_MakeAvailable(Tether)
    endif()
endfunction()

DeclareLibs()
MakeLibsAvailable()