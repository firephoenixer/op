cmake_minimum_required(VERSION 3.5)

ADD_DEFINITIONS(-D _UNICODE)
ADD_DEFINITIONS(-D UNICODE)

ADD_DEFINITIONS(-D _CMAKE_BUILD)

SET(SRC_FILES "main.cpp"
"../libop/com/op_i.c")

include_directories( 
  ./
  ../libop/3rd_party/include/3rd_party
) 

add_executable(op_test ${SRC_FILES}  )

# 链接到DLL的导入库
# 注意：当DLL生成时，导入库会自动生成
IF(CMAKE_CL_64)
  # 直接链接到DLL文件
link_directories(${CMAKE_BINARY_DIR}/libop/${CMAKE_BUILD_TYPE})
ELSE(CMAKE_CL_64)
  link_directories(${CMAKE_BINARY_DIR}/libop/${CMAKE_BUILD_TYPE})
ENDIF(CMAKE_CL_64)

target_link_libraries(op_test op_x64)

IF(CMAKE_CL_64)
  install(TARGETS op_test  DESTINATION "${PROJECT_SOURCE_DIR}/bin/x64")
ELSE(CMAKE_CL_64)
  install(TARGETS op_test  DESTINATION "${PROJECT_SOURCE_DIR}/bin/x86")
ENDIF(CMAKE_CL_64)