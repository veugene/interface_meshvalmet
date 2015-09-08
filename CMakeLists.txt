project(compare_meshes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# Include plugin headers
include_directories(src src/wrapper)
  
# Include src/MeshValmet headers
include_directories(
  src/MeshValmet/lib3d
  src/MeshValmet/mesh
  src/InterBinaryShapeBasedInterpolationImageFilter
  ${include_directories}
)

# Include some helpful ITK mesh processing code
include_directories(
  src/itkQuadEdgeMeshProcessing
  ${include_directories}
)

set(SOURCE_FILES
    src/compare_meshes.cpp
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
    src/itkQuadEdgeMeshProcessing/itkMeshTovtkPolyData.cxx
    )

add_executable(compare_meshes ${SOURCE_FILES})
target_link_libraries(compare_meshes ${VTK_LIBRARIES} ${ITK_LIBRARIES} )
