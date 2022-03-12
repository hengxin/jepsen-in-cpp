add_rules("mode.debug", "mode.release")

add_requires("libssh2")
add_requires("log4cplus")
add_requires("jsoncpp")
add_requires("boost")
-- boost (in ubuntu, we use apt-get install but there may be some errors in windows)

set_languages("cxx17")

-- Src
target("control")
    set_kind("binary")
    add_includedirs("src/jepsen")
    add_files("src/jepsen/*.cpp")
    add_files("src/main.cpp")
    add_packages("libssh2")
    add_packages("log4cplus")
    add_packages("jsoncpp")
    add_packages("boost")
    add_syslinks("boost_system")
    add_syslinks("boost_filesystem")
    if is_plat("linux") then
        add_syslinks("pthread")
    end    

-- 2. Test
--- 2.1 Test source
---- Test for operation
target("test-operation")
    set_kind("binary")
    add_files("test/operation/*.cpp")
    add_files("src/jepsen/Operation.cpp")
    add_includedirs("src/jepsen")
    add_packages("jsoncpp")
    add_packages("log4cplus")

target("test-runner")
    set_kind("binary")
    add_files("test/runner/*.cpp")
    add_files("test/etcd/*.cpp")
    add_files("src/jepsen/*.cpp")
    add_includedirs("src/jepsen")
    add_includedirs("test/etcd")
    add_packages("jsoncpp")
    add_packages("log4cplus")
    add_packages("libssh2")
    add_packages("boost")
    add_syslinks("boost_system")
    add_syslinks("boost_filesystem")

target("test-worker")
    set_kind("binary")
    add_files("test/worker/*.cpp")
    add_files("src/jepsen/*.cpp")
    add_includedirs("src/jepsen")
    add_packages("jsoncpp")
    add_packages("log4cplus")
    add_packages("libssh2")
    add_packages("boost")
    add_syslinks("boost_filesystem")
    add_syslinks("boost_system")

target("test-remote")
    set_kind("binary")
    add_files("test/remote/*.cpp")
    add_files("src/jepsen/*.cpp")
    add_includedirs("src/jepsen")
    add_packages("jsoncpp")
    add_packages("log4cplus")
    add_packages("libssh2")
    add_packages("boost")
    add_syslinks("boost_filesystem")
    add_syslinks("boost_system")
    
--- 2.2 Test Third Party
---- Test for libssh2
target("test-libssh2-ssh2")
    set_kind("binary")
    add_files("test/libssh2/ssh2.cpp")
    add_packages("libssh2")

target("test-libssh2-ssh2-exec")
    set_kind("binary")
    add_files("test/libssh2/ssh2_exec.cpp")
    add_packages("libssh2")

---- Test for log4cplus
target("test-log4cplus")
    set_kind("binary")
    add_files("test/log4cplus/hello-world.cpp")
    add_packages("log4cplus")

---- Test for jsoncpp
target("test-jsoncpp")
    set_kind("binary")
    add_files("test/jsoncpp/main.cpp")
    add_packages("jsoncpp")


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

