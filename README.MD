# Parakeet

对 unlock-music 的非官方实现，使用 wxWidgets 作为基础 GUI 库开发。

![主界面截图](./.github/assets/app-main.png)

链接：[支持的格式一览][sup_format]

[sup_format]: https://github.com/jixunmoe/um-desktop/wiki/%E6%94%AF%E6%8C%81%E7%9A%84%E6%A0%BC%E5%BC%8F

## 下载

自动构建的文件可以在 GitHub Action 产生的文件内找到。

|  名称  | 链接                        |
| :----: | :-------------------------- |
| Win64  | [win64-build.zip][dl_win64] |
| 语言包 | [locale.zip][dl_locale]     |

[dl_locale]: https://nightly.link/jixunmoe/um-desktop/workflows/build-win/main/locale.zip
[dl_win64]: https://nightly.link/jixunmoe/um-desktop/workflows/build-win/main/win64-build.zip

## 构建说明

### Windows 下构建

- 安装 CMake
- 安装 Visual Studio 2019

```powershell
# 初始化 vcpkg
.\vcpkg\bootstrap-vcpkg.bat

 # 调试版
cmake --preset win64-2019-dbg
cmake --build --preset win64-2019-dbg

# 发布版
cmake --preset win64-2019-rel
cmake --build --preset win64-2019-rel
```

### Linux (Ubuntu 22.04) 下构建 (vcpkg)

- 安装工具链： `sudo apt-get install build-essential cmake ninja-build`
- 安装下述构建依赖：

  ```sh
  sudo apt-get install \
    libgcrypt20-dev \
    libgtk-3-dev \
    libjpeg-dev \
    libpng-dev \
    libsecret-1-dev \
    libsystemd-dev \
    libtiff-dev \
    libwebp-dev \
    libzstd-dev \
    pkg-config
  ```

- 执行 `bash ./prepare.linux.sh` 进行初始化。

#### Linux (Ubuntu 22.04)

- 安装工具链： `sudo apt-get install build-essential cmake ninja-build`
- 安装下述构建依赖（可能有缺失，如果编译失败请发 issue）：

  ```sh
  sudo apt-get install \
    libboost-filesystem-dev \
    libboost-nowide-dev \
    libboost-thread-dev \
    libgcrypt20-dev \
    libgtk-3-dev \
    libjpeg-dev \
    libpng-dev \
    libssl-dev \
    libsecret-1-dev \
    libsystemd-dev \
    libtiff-dev \
    libwebp-dev \
    libzstd-dev \
    pkg-config
  ```
