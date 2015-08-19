#include <unistd.h>
#include <iostream>

#include <vtkGenericDataObjectReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>

#include "VTK_to_MeshValmet.h"
#include "CompareMeshes.h"

int main()
{
  // Read provided sample meshes from file 
  std::string filename1 = "sample_mesh_seg.vtk";
  vtkSmartPointer<vtkGenericDataObjectReader> reader1 = vtkSmartPointer<vtkGenericDataObjectReader>::New();
  reader1->SetFileName( filename1.c_str() );
  reader1->Update();
  vtkSmartPointer<vtkPolyData> polydata1 = reader1->GetPolyDataOutput();
  
  std::string filename2 = "sample_mesh_gt.vtk";
  vtkSmartPointer<vtkGenericDataObjectReader> reader2 = vtkSmartPointer<vtkGenericDataObjectReader>::New();
  reader2->SetFileName( filename2.c_str() );
  reader2->Update();
  vtkSmartPointer<vtkPolyData> polydata2 = reader2->GetPolyDataOutput();
  
  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetInputData(polydata1);
  writer->SetFileName("out1.vtk");
  writer->Update();
  writer->SetInputData(polydata2);
  writer->SetFileName("out2.vtk");
  writer->Update();
  
  // Compare meshes using MeshValmet
  CompareMeshes* cm = new CompareMeshes();
  CompareMeshes::mesh_differences diff = cm->GetMeshDifferences( VTK_to_MeshValmet(polydata1),
                                                                  VTK_to_MeshValmet(polydata2) );
  
  // Output results
  std::cout << "MIN DIST: " << diff.min_dist << std::endl;
  std::cout << "MAX DIST: " << diff.max_dist << std::endl;
  std::cout << "MIN ABS DIST: " << diff.abs_min_dist << std::endl;
  std::cout << "MAX ABS DIST: " << diff.abs_max_dist << std::endl;
  std::cout << "MEAN DIST: " << diff.mean_dist << std::endl;
  std::cout << "MEAN ABS DIST: " << diff.abs_mean_dist << std::endl;
  std::cout << "RMS DIST: " << diff.rms_dist << std::endl;
  std::cout << "VOLUME OVERLAP: " << diff.volume_overlap << std::endl;
  std::cout << "INTERSECTION/UNION: " << diff.int_union_ratio << std::endl;
  
  return 0;
}