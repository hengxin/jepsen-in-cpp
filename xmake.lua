set_project("Jepsen-in-cpp")
set_version("0.0.1", {build = "%Y%m%d%H%M"})

add_rules("mode.debug", "mode.release")

add_requires("libssh2")
add_requires("log4cplus")
add_requires("jsoncpp")
add_requires("boost")
add_requires("concurrentqueue")
add_requires("tbb")

set_languages("cxx17")

-- Configuration
target("config")
    includes("check_cxxincludes.lua")
    includes("check_cxxfuncs.lua")
    includes("check_macros.lua")
    -- Platform checks
    -- From libssh2, refer to https://github.com/libssh2/libssh2/blob/master/example/CMakeLists.txt
    add_files("config/main.cpp")
    add_includedirs("config")
    set_configdir("config")
    add_configfiles("config/libssh2_config.h.in", {filename = "libssh2_config.h"})
    configvar_check_cxxincludes("HAVE_INTTYPES_H", "inttypes.h")
    configvar_check_cxxincludes("HAVE_UNISTD_H", "unistd.h")
    configvar_check_cxxincludes("HAVE_STDLIB_H", "stdlib.h")
    configvar_check_cxxincludes("HAVE_SYS_SELECT_H", "sys/select.h")
    configvar_check_cxxincludes("HAVE_SYS_SOCKET_H", "sys/socket.h")
    configvar_check_cxxincludes("HAVE_SYS_TIME_H", "sys/time.h")
    configvar_check_cxxincludes("HAVE_ARPA_INET_H", "arpa/inet.h")
    configvar_check_cxxincludes("HAVE_NETINET_IN_H", "netinet/in.h")
    configvar_check_cxxincludes("HAVE_WINSOCK2_H", "winsock2.h")

    configvar_check_cxxfuncs ("HAVE_STRCASECMP", "strcasecmp", {includes = {"strings.h"}})
    configvar_check_cxxfuncs ("HAVE__STRICMP", "_stricmp", {includes = {"string.h"}})
    configvar_check_cxxfuncs ("HAVE_SNPRINTF", "snprintf", {includes = {"stdio.h"}})
    configvar_check_cxxfuncs ("HAVE__SNPRINTF", "_snprintf", {includes = {"stdio.h"}})

    configvar_check_macros("HAVE___FUNC__", "__func__")
    configvar_check_macros("HAVE___FUNCTION__", "__FUNCTION__")
    configvar_check_macros("HAVE___PRETTY_FUNCTION__", "__PRETTY_FUNCTION__")

-- Src
-- target("control")
--     set_kind("binary")
--     add_includedirs("src/jepsen")
--     add_files("src/jepsen/*.cpp")
--     add_files("src/main.cpp")
--     add_packages("libssh2", "log4cplus", "jsoncpp", "boost", "concurrentqueue")
--     add_syslinks("boost_system", "boost_filesystem")
--     if is_plat("linux") then
--         add_syslinks("pthread")
--     end    
--     add_syslinks("tbb")


-- 2. Test
--- 2.1 Test source
---- Test for operation
target("test-operation")
    set_kind("binary")
    add_files("test/operation/*.cpp")
    add_files("src/jepsen/Operation.cpp")
    add_includedirs("src/jepsen")
    add_packages("jsoncpp", "log4cplus", "concurrentqueue")
    add_syslinks("tbb")

target("test-runner")
    set_kind("binary")
    add_files("test/runner/*.cpp")
    add_files("test/etcd/ETCD.cpp")
    add_files("src/jepsen/*.cpp", "src/jepsen/utils/*.cpp", "src/jepsen/generator/*.cpp")
    add_includedirs("src/jepsen")
    add_includedirs("src/jepsen/include", "src/jepsen/utils", "src/jepsen/generator")
    add_includedirs("test/etcd")
    add_includedirs("config")
    add_packages("jsoncpp", "log4cplus", "libssh2", "boost", "concurrentqueue", "tbb")
    add_syslinks("tbb")
    if is_plat("linux", "macosx") then
        add_defines("LOG4CPLUS_CONFIG=\"/home/young/github-projects/jepsen-in-cpp/config/log4cplus.cfg\"")
        add_syslinks("pthread")
        add_syslinks("boost_system", "boost_filesystem")
        add_syslinks("etcd-cpp-api", "protobuf", "grpc++", "z", "cpprest", "ssl", "crypto")
    end
    if is_plat("windows") then
        add_defines("LOG4CPLUS_CONFIG=\"D://Education//Programs//Cpp//jepsen-in-cpp//config//log4cplus.cfg\"")
        add_syslinks("libboost_filesystem-mt-s")
    end

target("test-worker")
    set_kind("binary")
    add_files("test/worker/*.cpp")
    add_files("test/etcd/*.cpp")
    add_files("src/jepsen/*.cpp", "src/jepsen/utils/*.cpp")
    add_includedirs("src/jepsen")
    add_includedirs("src/jepsen/include", "src/jepsen/utils")
    add_includedirs("test/etcd")
    add_packages("jsoncpp", "log4cplus", "libssh2", "boost", "concurrentqueue")
    add_syslinks("pthread")
    add_syslinks("tbb")
    add_syslinks("boost_system", "boost_filesystem")
    add_syslinks("etcd-cpp-api", "protobuf", "grpc++", "z", "cpprest", "ssl", "crypto", "boost_system")

target("test-remote")
    set_kind("binary")
    add_files("test/remote/*.cpp")
    add_files("src/jepsen/*.cpp", "src/jepsen/utils/*.cpp")
    add_includedirs("src/jepsen")
    add_includedirs("src/jepsen/include", "src/jepsen/utils")
    add_packages("jsoncpp", "log4cplus", "libssh2", "boost", "concurrentqueue")
    add_syslinks("pthread")
    add_syslinks("boost_filesystem", "boost_system")
    add_syslinks("tbb")
    
--- 2.2 Test Third Party
---- Test for libssh2
target("test-libssh2-ssh2")
    set_kind("binary")
    add_includedirs("config")
    add_files("test/libssh2/ssh2.cpp")
    add_packages("libssh2")

target("test-libssh2-ssh2-exec")
    set_kind("binary")
    add_includedirs("config")
    add_files("test/libssh2/ssh2_exec.cpp")
    add_packages("libssh2")

---- Test for log4cplus
target("test-log4cplus")
    set_kind("binary")
    add_files("test/log4cplus/hello-world.cpp")
    add_packages("log4cplus")

target("test-log4cplus-file")
    set_kind("binary")
    add_files("test/log4cplus/file.cpp")
    add_packages("log4cplus")

---- Test for jsoncpp
target("test-jsoncpp")
    set_kind("binary")
    add_files("test/jsoncpp/main.cpp")
    add_packages("jsoncpp")

---- Test for etcd-apiv3
target("test-etcd-apiv3")
    set_kind("binary")
    add_files("test/etcd-driver/*.cpp")
    add_syslinks("pthread")
    add_syslinks("etcd-cpp-api", "protobuf", "grpc++", "z", "cpprest", "ssl", "crypto", "boost_system")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro defination
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

