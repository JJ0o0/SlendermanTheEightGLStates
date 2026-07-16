#!/bin/bash
set -e

# Dependências do sistema (Arch Linux)
# mingw-w64-gcc: o compilador cross
# wine: pra rodar o .exe gerado sem sair do Linux
# git: pro vcpkg baixar/atualizar os pacotes
echo ">> Verificando dependências (mingw-w64-gcc, wine, git)..."
sudo pacman -S --needed mingw-w64-gcc wine git

# vcpkg: gerenciador de pacotes C++ que vai buildar glfw3 e glm
# especificamente para o "triplet" x64-mingw-static (Windows 64-bit,
# linkado estaticamente -> não precisa distribuir DLLs extras do MinGW)
if [ ! -d "vcpkg" ]; then
    echo ">> Clonando vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git
    ./vcpkg/bootstrap-vcpkg.sh
fi

echo ">> Instalando glfw3 e glm para Windows (triplet x64-mingw-static)..."
./vcpkg/vcpkg install glfw3 glm --triplet x64-mingw-static

# Toolchain file do MinGW
echo ">> Gerando mingw-w64.cmake (toolchain file)..."
cat > mingw-w64.cmake << 'EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF

# Configurar o build com CMake
echo ">> Configurando build (build-windows/)..."
cmake -B build-windows -S . \
    -DCMAKE_TOOLCHAIN_FILE="$(pwd)/vcpkg/scripts/buildsystems/vcpkg.cmake" \
    -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$(pwd)/mingw-w64.cmake" \
    -DVCPKG_TARGET_TRIPLET=x64-mingw-static \
    -DCMAKE_BUILD_TYPE=Release

# Compilar
echo ">> Compilando..."
cmake --build build-windows --parallel "$(nproc)"

# Localizar o .exe gerado
EXE_PATH="build-windows/SlendermanTEGS.exe"

if [ ! -f "$EXE_PATH" ]; then
    echo "!! Não achei $EXE_PATH — confira o nome do executável em build-windows/"
    exit 1
fi

# Copiando assets
rm -rf build-windows/assets
rsync -a assets/ build-windows/assets/

echo ">> Build concluído: $EXE_PATH"