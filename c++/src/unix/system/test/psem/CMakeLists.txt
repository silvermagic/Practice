# 收集当前目录下的所有 .cpp 文件，每个文件生成一个可执行程序
file(GLOB cpp_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(fmt REQUIRED)

foreach(cpp_file ${cpp_files})
    # 获取文件名（不含路径和扩展名）作为可执行程序名
    get_filename_component(exec_name ${cpp_file} NAME_WE)

    # 创建可执行程序
    add_executable(${exec_name} ${cpp_file})

    # 链接共享库（C 库）和 fmt 库
    target_link_libraries(${exec_name} 
        PRIVATE
        fmt::fmt
        unix_system
    )
    
    # 设置可执行程序的输出目录
    set_target_properties(${exec_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

