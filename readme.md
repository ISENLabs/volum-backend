# Volum-Backend


## Dev instructions:
> [!TIP]
> I highly recommend you building this software in a Linux/MacOS system. If you're using Windows, consider building this in WSL
- Install VCPKG on your system ([VCPKG Documentation](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell))
- Run `vcpkg install` in the current directory to install all dependencies.
- You *may* need to edit your favourite IDE config to look up for headers in VCPKG install dir : `${VCPKG_ROOT}/installed/arm64-ose/include/**`

## Build instructions:
> [!TIP]
> I highly recommend you building this software in a Linux/MacOS system. If you're using Windows, consider building this in WSL
- Install VCPKG on your system ([VCPKG Documentation](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell))
- Install Ninja ([Ninjas' releases](https://github.com/ninja-build/ninja/releases))
- Launch run.sh script: `bash run.sh`

## Notices
> As libmariadb v3.4.x requires to have the MariaDB ssl enabled by default, you might need to install a previous version of vcpkg. For instance, `9a6da16845eca8d6ed70be416c1acbd206894c7f` is working well