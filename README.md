# jepsen-in-cpp
A CPP Version of [Jepsen](https://github.com/jepsen-io/jepsen)

## Requirement
- TBB:  oneAPI Threading Building Blocks (oneTBB). 
  - `apt-get install libtbb-dev` 
  - tbb can be installed by xmake, but how to solve the github:443 error is a dirty work, so I use apt as a replacement now. In the future, I will take this into consideration.


## Build
This project is build by [xmake](https://github.com/tboox/xmake) which is a cross-platform build utility based on Lua.
### 1. Install Xmake
The installation of xmake can refer to https://xmake.io/#/guide/installation.

In Ubuntu, you can install it easily via apt
```bash
sudo add-apt-repository ppa:xmake-io/xmake
sudo apt update
sudo apt install xmake
```
### 2. Build 
Via xmake, you can easily build this project by just
```bash
xmake
```
All of the dependencies are set in `xmake.lua` 
and they will installed automatically.
The built packages can be found in `~/.xmake/packages`. 
If you want to add the include pathes of some package in you vscode or IDEs, 
take boost for example, you can use 
```bash
xrepo fetch boost
```
All the information will list
```bash
$ xrepo fetch boost
{ 
  { 
    license = "BSL-1.0",
    includedirs = { 
      "/home/young/.xmake/packages/b/boost/1.78.0/edc6c426909a427395ddd1d79622c780/include" 
    },
    version = "1.78.0",
    syslinks = { 
      "pthread",
      "dl" 
    },
    linkdirs = { 
      "/home/young/.xmake/packages/b/boost/1.78.0/edc6c426909a427395ddd1d79622c780/lib" 
    },
    links = { 
      "boost_filesystem-mt",
      "boost_atomic-mt" 
    },
    libfiles = { 
      "/home/young/.xmake/packages/b/boost/1.78.0/edc6c426909a427395ddd1d79622c780/lib/libboost_filesystem-mt.a",
      "/home/young/.xmake/packages/b/boost/1.78.0/edc6c426909a427395ddd1d79622c780/lib/libboost_atomic-mt.a" 
    },
    static = true 
  } 
}

```
### 3. VSCode Setting


## Design
- How to explain the test map in jepsen to this project.

## TODO

## Reference
- [xmake使用初体验](https://zhuanlan.zhihu.com/p/473205876)
- [Log4Cplus 学习笔记 - 配置文件的使用](https://blog.csdn.net/kakiebu/article/details/105501833?spm=1001.2014.3001.5506)