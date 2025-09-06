# Subdir CMake (versão mínima só por clareza)
cmake_minimum_required(VERSION 3.16)

# Raiz do projeto e pasta de exemplos
set(PROJECT_ROOT  "${CMAKE_SOURCE_DIR}")
set(EXAMPLES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# ---------- Função: localizar diretórios que contêm .cpp ----------
function(find_example_directories root_dir result_var)
    set(dirs_with_cpp "")
    file(GLOB_RECURSE cpp_files "${root_dir}/*.cpp")
    foreach(cpp_file ${cpp_files})
        get_filename_component(dir_path "${cpp_file}" DIRECTORY)
        list(APPEND dirs_with_cpp "${dir_path}")
    endforeach()
    list(REMOVE_DUPLICATES dirs_with_cpp)
    set(${result_var} ${dirs_with_cpp} PARENT_SCOPE)
endfunction()

# Buscar diretórios com exemplos
find_example_directories("${EXAMPLES_ROOT}" EXAMPLE_DIRS_WITH_CPP)

if(NOT EXAMPLE_DIRS_WITH_CPP)
    message(STATUS "  [Examples] Nenhum exemplo encontrado.")
else()
    message(STATUS "  [Examples] Exemplos detectados:")
endif()

# Alvo global para compilar todos os exemplos
add_custom_target(examples ALL COMMENT "Compilando todos os exemplos")

# Caminho de includes da biblioteca
set(SOLIDLAB_INCLUDE_DIR "${PROJECT_ROOT}/SolidLabLib/include")

foreach(dir_path ${EXAMPLE_DIRS_WITH_CPP})
    # Nome do exemplo = último componente do caminho
    get_filename_component(example_name "${dir_path}" NAME)

    # Nome base amigável do alvo: exe_<pasta>
    set(base_target_name "exe_${example_name}")
    set(target_name "${base_target_name}")

    # Garantir unicidade: se já existe, anexa um hash curto do caminho relativo
    file(RELATIVE_PATH rel_dir "${EXAMPLES_ROOT}" "${dir_path}")
    if(TARGET "${target_name}")
        string(MD5 _hash "${rel_dir}")
        string(SUBSTRING "${_hash}" 0 8 _short)
        set(target_name "${base_target_name}__${_short}")
    endif()

    # Todas as fontes .cpp do diretório
    file(GLOB example_sources "${dir_path}/*.cpp")

    message(STATUS "    - ${dir_path}  -> alvo: ${target_name}")

    # 1) Executável do exemplo (um por diretório)
    add_executable(${target_name} ${example_sources})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${dir_path}"  # binário ao lado do .cpp
        OUTPUT_NAME "${example_name}"           # nome do exe = nome da pasta
    )

    # 2) Incluir cabeçalhos e linkar com a lib SolidLab
    target_include_directories(${target_name} PRIVATE
        "${SOLIDLAB_INCLUDE_DIR}"
    )
    target_link_libraries(${target_name} PRIVATE SolidLab)

    # 3) Flags de otimização (apenas em Release)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} PRIVATE -O3 -march=native)
        elseif(MSVC)
            target_compile_options(${target_name} PRIVATE /O2)
        endif()
    endif()

    # 4) Acrescenta ao alvo global "examples"
    add_dependencies(examples ${target_name})

    # 5) Alvo para executar este exemplo
    set(run_target "run_${target_name}")
    add_custom_target(${run_target}
        DEPENDS ${target_name}
        COMMAND "${dir_path}/${example_name}"
        WORKING_DIRECTORY "${dir_path}"
        COMMENT "Executando ${example_name} (${dir_path})"
    )

    # 6) Integra com "run_examples" da raiz (se existir)
    if(TARGET run_examples)
        add_dependencies(run_examples ${target_name})
        add_dependencies(run_examples ${run_target})
    endif()
endforeach()

# Alvo para executar todos os exemplos (encadeia todos os run_<alvo>)
if(EXAMPLE_DIRS_WITH_CPP)
    add_custom_target(run_all
        COMMENT "Execute cada exemplo individualmente com: cmake --build build --target run_<alvo>"
    )
    foreach(dir_path ${EXAMPLE_DIRS_WITH_CPP})
        get_filename_component(example_name "${dir_path}" NAME)
        set(base_target_name "exe_${example_name}")

        file(RELATIVE_PATH rel_dir "${EXAMPLES_ROOT}" "${dir_path}")
        set(target_name "${base_target_name}")
        if(TARGET "${target_name}")
            # ok
        else()
            # se colidiu no loop anterior, o alvo terá sufixo de hash
            string(MD5 _hash "${rel_dir}")
            string(SUBSTRING "${_hash}" 0 8 _short)
            set(target_name "${base_target_name}__${_short}")
        endif()

        add_dependencies(run_all "run_${target_name}")
    endforeach()
endif()
