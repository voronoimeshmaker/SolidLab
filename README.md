# SolidLab

Biblioteca didática em **C++20** (sem dependências externas por padrão) para treinar SOLID, TDD e padrões de projeto.  
Estrutura: **headers** em `SolidLabLib/include/` e **fontes** em `SolidLabLib/src/`.  
A biblioteca compilada é emitida em **`./bin/`** (configurado no `CMakeLists.txt`).  
Exemplos e testes são opcionais (via flags do CMake).

---

## Requisitos de compilação

### Obrigatórios
- **CMake ≥ 3.10**
- **Compilador C++20**
  - **Linux**: GCC ≥ 10 ou Clang ≥ 10
  - **macOS**: LLVM/Clang ≥ 12 (recomendado via Homebrew)
  - **Windows**: **MSVC** (VS 2019 16.10+), **ou** MinGW-w64 (GCC), **ou** LLVM/Clang
- **Make** ou **Ninja** (qualquer um)

> O `CMakeLists.txt` liga `-fopenmp` **apenas** para GCC/Clang (Linux/macOS).
> Em MSVC **não** habilita OpenMP por padrão (não é necessário para compilar o projeto).

### Opcionais (somente se `-DBUILD_TESTS=ON`)
- **GoogleTest (GTest)**

**Linux (Ubuntu/Debian)**
```bash
sudo apt update
sudo apt install -y cmake build-essential
# (Opcional p/ testes)
sudo apt install -y libgtest-dev
sudo cmake -S /usr/src/googletest -B /usr/src/googletest/build
sudo cmake --build /usr/src/googletest/build --config Release
sudo cmake --install /usr/src/googletest/build
