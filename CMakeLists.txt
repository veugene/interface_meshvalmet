project(example)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# Include plugin headers
include_directories(src src/wrapper)
  
# Include src/MeshValmet headers
include_directories(
  src/MeshValmet/lib3d
  src/MeshValmet/mesh
  src/InterBinaryShapeBasedInterpolationImageFilter
  ${include_directories}
)

set(SOURCE_FILES
    src/example.cpp
    src/wrapper/CompareMeshes.cpp
    src/wrapper/vtkPLYStreamWriter.cpp
    src/wrapper/VTK_to_MeshValmet.h
    src/MeshValmet/lib3d/block_list.cxx
    src/MeshValmet/lib3d/model_in.cxx
    src/MeshValmet/lib3d/model_in_ply.cxx
    src/MeshValmet/lib3d/model_in_raw.cxx
    src/MeshValmet/lib3d/model_in_smf.cxx
    src/MeshValmet/lib3d/model_in_vrml_iv.cxx
    src/MeshValmet/mesh/compute_error.cxx
    src/MeshValmet/mesh/compute_volume_overlap.cxx
    src/MeshValmet/mesh/xalloc.cxx
    src/MeshValmet/mesh/reporting.cxx
    )

add_executable(example ${SOURCE_FILES})
target_link_libraries(example ${VTK_LIBRARIES} )
