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