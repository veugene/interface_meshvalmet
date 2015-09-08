#include <unistd.h>
#include <iostream>

#include <vtkGenericDataObjectReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>

#include <itkImageFileReader.h>
#include <itkBinaryMask3DMeshSource.h>
#include <itkSimplexMeshToTriangleMeshFilter.h>

#include "itkMeshTovtkPolyData.h"
#include "VTK_to_MeshValmet.h"
#include "CompareMeshes.h"

template < typename TPixel, unsigned int VImageDimension >
void RunCompareMeshes(std::string filename_image, std::string filename_mesh);

int main(int argc, char** argv)
{
  if( argc != 3 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " <mesh filename> <ground truth image filename>" << std::endl;
    return 1;
  }
  
  // Get mesh filename
  std::string filename_mesh = argv[1];
  
  // Identify image type and load it
  std::string filename_image = argv[2];
  
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO( filename_image.c_str(), itk::ImageIOFactory::ReadMode );
  imageIO->SetFileName(filename_image);
  imageIO->ReadImageInformation();
  itk::ImageIOBase::IOPixelType PixelType = imageIO->GetPixelType();
  itk::ImageIOBase::IOComponentType PixelValueType = imageIO->GetComponentType();
  
  // Supporting DICOM, MetaImage (see http://www.itk.org/Wiki/ITK/File_Formats):
  if( PixelType == itk::ImageIOBase::SCALAR )
  {
    switch( PixelValueType )
    {
      case itk::ImageIOBase::FLOAT  :
        RunCompareMeshes<float, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::DOUBLE  :
        RunCompareMeshes<double, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::CHAR  :
        RunCompareMeshes<char, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::UCHAR  :
        RunCompareMeshes<unsigned char, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::SHORT  :
        RunCompareMeshes<short, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::USHORT  :
        RunCompareMeshes<unsigned short, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::LONG  :
        RunCompareMeshes<long, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::ULONG  :
        RunCompareMeshes<unsigned long, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::INT  :
        RunCompareMeshes<int, 3>(filename_image, filename_mesh);
        break;
        
      case itk::ImageIOBase::UINT  :
        RunCompareMeshes<unsigned int, 3>(filename_image, filename_mesh);
        break;
        
      default:
        std::cerr << "Unsupported pixel value type: " << imageIO->GetComponentTypeAsString( PixelValueType )
        << std::endl;
        return 2;
    }
  }
  else
  {
    std::cerr << "File format not supported. Only DICOM and MetaImage files are supported.\n";
    std::cerr << "Unsupported pixel type: " << imageIO->GetPixelTypeAsString( PixelType ) << std::endl;
  }
  
  return 0;
}

template < typename TPixel, unsigned int VImageDimension >
void RunCompareMeshes(std::string filename_image, std::string filename_mesh)
{
  // Load image
  typedef itk::Image< TPixel, VImageDimension > ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename_image );
  reader->Update();
  
  // Convert the image into a mesh
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> TriangleMeshTraits;
  typedef itk::Mesh<double,3, TriangleMeshTraits> TriangleMeshType;
  typedef itk::BinaryMask3DMeshSource< ImageType, TriangleMeshType > MaskToMeshType;
  typename MaskToMeshType::Pointer mask2mesh = MaskToMeshType::New();
  mask2mesh->SetInput( reader->GetOutput() );
  mask2mesh->Update();
  if( mask2mesh->GetOutput()->GetNumberOfPoints()==0 )
  {
    std::cerr << "Error: No points in the mesh." << std::endl;
    return;
  }
  itkMeshTovtkPolyData itk2vtk;
  itk2vtk.SetInput( mask2mesh->GetOutput() );
  vtkSmartPointer<vtkPolyData> mesh1 = itk2vtk.GetOutput();
  
  // Read mesh2 from file
//   vtkSmartPointer<vtkGenericDataObjectReader> reader1 = vtkSmartPointer<vtkGenericDataObjectReader>::New();
  vtkSmartPointer<vtkXMLPolyDataReader> reader1 = vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader1->SetFileName( filename_mesh.c_str() );
  reader1->Update();
//   vtkSmartPointer<vtkPolyData> mesh2 = reader1->GetPolyDataOutput();
  vtkSmartPointer<vtkPolyData> mesh2 = reader1->GetOutput();
  
  // Compare meshes using MeshValmet
  CompareMeshes* cm = new CompareMeshes();
  CompareMeshes::mesh_differences diff = cm->GetMeshDifferences( VTK_to_MeshValmet(mesh1), VTK_to_MeshValmet(mesh2) );
  
//   // Output results
//   std::cout << "MIN DIST: " << diff.min_dist << std::endl;
//   std::cout << "MAX DIST: " << diff.max_dist << std::endl;
//   std::cout << "MIN ABS DIST: " << diff.abs_min_dist << std::endl;
//   std::cout << "MAX ABS DIST: " << diff.abs_max_dist << std::endl;
//   std::cout << "MEAN DIST: " << diff.mean_dist << std::endl;
//   std::cout << "MEAN ABS DIST: " << diff.abs_mean_dist << std::endl;
//   std::cout << "RMS DIST: " << diff.rms_dist << std::endl;
//   std::cout << "VOLUME OVERLAP: " << diff.volume_overlap << std::endl;
//   std::cout << "INTERSECTION/UNION: " << diff.int_union_ratio << std::endl;
  
  std::string results_filename = "results.txt";
  FILE* results_file = fopen(results_filename.c_str(), "aw");
  fprintf(results_file, "%s %s %s %s %s %s %s %s %s\n", "min_dist", "max_dist", "min_abs_dist", "max_abs_dist",
          "mean_dist", "mean_abs_dist", "rms_dist", "volume_overlap", "int_over_union");
  fprintf(results_file, "%f %f %f %f %f %f %f %f %f\n", diff.min_dist, diff.max_dist,
          diff.abs_min_dist, diff.abs_max_dist, diff.mean_dist, diff.abs_mean_dist,
          diff.rms_dist, diff.volume_overlap, diff.int_union_ratio);
  fclose(results_file);
}