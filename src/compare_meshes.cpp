/* Author: Eugene Vorontsov
 *
 * Load any two files of type image mask (target value 1, else 0) or vtk/vtp mesh and compare the two surfaces using MeshValmet.
 * Code for converting itk to vtk meshes taken from Arnaud Gelas <arnaud_gelas@hms.harvard.edu> from github (https://github.com/arnaudgelas/itkQuadEdgeMeshProcessing) */

#include <unistd.h>
#include <iostream>

#include <vtkGenericDataObjectReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>

#include <itkImageFileReader.h>
#include <itkCastImageFilter.h>
#include <itkBinaryMask3DMeshSource.h>
#include <itkSimplexMeshToTriangleMeshFilter.h>

#include "itkMeshTovtkPolyData.h"
#include "VTK_to_MeshValmet.h"
#include "CompareMeshes.h"

enum file_type {IMAGE, VTK, VTP, UNKNOWN};

/* Identify file type: ITK image, vtk/vtp mesh, unkown. */
file_type identify_file_type(std::string filename)
{
  // Check for image (by header)
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO( filename.c_str(), itk::ImageIOFactory::ReadMode );
  if( imageIO.IsNotNull() )
  {
    return IMAGE;
  }
  
  // Check for mesh (by file extension)
  std::string::size_type idx = filename.rfind('.');
  if(idx != std::string::npos)
  {
    std::string extension = filename.substr(idx+1);
    if(extension.compare("vtk")==0)
    {
      return VTK;
    }
    if(extension.compare("vtp")==0)
    {
      return VTP;
    }
  }
  
  // Unknown file type
  return UNKNOWN;
}




/* Load an image and return the image with float type pixels.
   Supporting DICOM, MetaImage (see http://www.itk.org/Wiki/ITK/File_Formats) */
template < typename TPixel, typename TCastPixel >
typename itk::Image<TCastPixel, 3>::Pointer load_and_cast_image3D_subroutine(std::string filename);
template < typename TCastPixel >
typename itk::Image<TCastPixel, 3>::Pointer load_and_cast_image3D(std::string filename)
{
  // Identify image type
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO( filename.c_str(), itk::ImageIOFactory::ReadMode );
  imageIO->SetFileName(filename);
  imageIO->ReadImageInformation();
  itk::ImageIOBase::IOPixelType PixelType = imageIO->GetPixelType();
  itk::ImageIOBase::IOComponentType PixelValueType = imageIO->GetComponentType();
  if( imageIO->GetNumberOfDimensions() != 3 )
  {
    std::cerr << "ERROR: There are fewer or more than 3 dimensions in the image file: " << filename << std::endl;
    throw 2;
  }
  
  // Load and cast the image
  if( PixelType == itk::ImageIOBase::SCALAR )
  {
    switch( PixelValueType )
    {
      case itk::ImageIOBase::FLOAT  :
        return load_and_cast_image3D_subroutine<float, TCastPixel>(filename);
      case itk::ImageIOBase::DOUBLE  :
        return load_and_cast_image3D_subroutine<double, TCastPixel>(filename);
      case itk::ImageIOBase::CHAR  :
        return load_and_cast_image3D_subroutine<char, TCastPixel>(filename);
      case itk::ImageIOBase::UCHAR  :
        return load_and_cast_image3D_subroutine<unsigned char, TCastPixel>(filename);
      case itk::ImageIOBase::SHORT  :
        return load_and_cast_image3D_subroutine<short, TCastPixel>(filename);
      case itk::ImageIOBase::USHORT  :
        return load_and_cast_image3D_subroutine<unsigned short, TCastPixel>(filename);
      case itk::ImageIOBase::LONG  :
        return load_and_cast_image3D_subroutine<long, TCastPixel>(filename);
      case itk::ImageIOBase::ULONG  :
        return load_and_cast_image3D_subroutine<unsigned long, TCastPixel>(filename);
      case itk::ImageIOBase::INT  :
        return load_and_cast_image3D_subroutine<int, TCastPixel>(filename);
      case itk::ImageIOBase::UINT  :
        return load_and_cast_image3D_subroutine<unsigned int, TCastPixel>(filename);
      default:
        std::cerr << "Unsupported pixel value type: " << imageIO->GetComponentTypeAsString( PixelValueType )
        << std::endl;
        throw 2;
    }
  }
  else
  {
    std::cerr << "File format not supported. Only DICOM and MetaImage files are supported.\n";
    std::cerr << "Unsupported pixel type: " << imageIO->GetPixelTypeAsString( PixelType ) << std::endl;
    throw 2;
  }
}

template < typename TPixel, typename TCastPixel >
typename itk::Image<TCastPixel, 3>::Pointer load_and_cast_image3D_subroutine(std::string filename)
{
  // Load the image
  typedef itk::Image< TPixel, 3 > LoadImageType;
  typedef itk::ImageFileReader<LoadImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( filename );
  reader->Update();
  
  // Cast the image to an internal type
  typedef itk::Image< TCastPixel, 3 > CastImageType;
  typedef itk::CastImageFilter< LoadImageType, CastImageType > CastFilterType;
  typename CastFilterType::Pointer cast_filter = CastFilterType::New();
  cast_filter->SetInput( reader->GetOutput() );
  cast_filter->Update();
  
  return cast_filter->GetOutput();
}


/* Load a file and return the contents as a mesh. If it's an mask, convert it to a mesh. */
vtkSmartPointer<vtkPolyData> load_file_as_mesh(std::string filename, file_type type)
{
  switch( type )
  {
    case IMAGE :
    {
      // Load and cast the image to an internal type
      typedef itk::Image< float, 3 > InternalImageType;
      InternalImageType::Pointer image = load_and_cast_image3D<float>(filename);
      
      // Convert the image into a mesh
      typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> TriangleMeshTraits;
      typedef itk::Mesh<double,3, TriangleMeshTraits> TriangleMeshType;
      typedef itk::BinaryMask3DMeshSource< InternalImageType, TriangleMeshType > MaskToMeshType;
      typename MaskToMeshType::Pointer mask2mesh = MaskToMeshType::New();
      mask2mesh->SetInput( image );
      mask2mesh->Update();
      if( mask2mesh->GetOutput()->GetNumberOfPoints()==0 )
      {
        std::cerr << "Error: No points in the mesh." << std::endl;
        throw 2;
      }
      itkMeshTovtkPolyData itk2vtk;
      itk2vtk.SetInput( mask2mesh->GetOutput() );
      return itk2vtk.GetOutput();
    } 
    case VTK :
    {
      vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
      reader->SetFileName( filename.c_str() );
      reader->Update();
      return reader->GetPolyDataOutput();
    }
    case VTP :
    {
      vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
      reader->SetFileName( filename.c_str() );
      reader->Update();
      return reader->GetOutput();
    }
    case UNKNOWN :
    {
      std::cerr << "ERROR: Cannot load file; unknown file type for file: " << filename << std::endl;
      throw 2;
    }
  }
}


int main(int argc, char** argv)
{
  if( argc!=3 and argc!=4 )
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " <mesh/image filename> <ground truth mesh/image filename> [<results file>]" << std::endl;
    return 1;
  }
  
  // Identify file types and load as meshes accordingly
  file_type type1 = identify_file_type(argv[1]);
  if( type1==UNKNOWN )
  {
    std::cerr << "Unknown file type for file: " << argv[1] << std::endl;
    return 1;
  }
  file_type type2 = identify_file_type(argv[2]);
  if( type2==UNKNOWN )
  {
    std::cerr << "Unknown file type for file: " << argv[2] << std::endl;
    return 1;
  }
  vtkSmartPointer<vtkPolyData> mesh1 = load_file_as_mesh(argv[1], type1);
  vtkSmartPointer<vtkPolyData> mesh2 = load_file_as_mesh(argv[2], type2);
  
  // Compare meshes using MeshValmet
  CompareMeshes* cm = new CompareMeshes();
  CompareMeshes::mesh_differences diff = cm->GetMeshDifferences( VTK_to_MeshValmet(mesh1), VTK_to_MeshValmet(mesh2) );
  
  // Output results to stdout
  std::cout << "MIN DIST: " << diff.min_dist << std::endl;
  std::cout << "MAX DIST: " << diff.max_dist << std::endl;
  std::cout << "MIN ABS DIST: " << diff.abs_min_dist << std::endl;
  std::cout << "MAX ABS DIST: " << diff.abs_max_dist << std::endl;
  std::cout << "MEAN DIST: " << diff.mean_dist << std::endl;
  std::cout << "MEAN ABS DIST: " << diff.abs_mean_dist << std::endl;
  std::cout << "RMS DIST: " << diff.rms_dist << std::endl;
  std::cout << "VOLUME OVERLAP: " << diff.volume_overlap << std::endl;
  std::cout << "INTERSECTION/UNION: " << diff.int_union_ratio << std::endl;
  
  // Output results to a file (compact)
  if( argc==4 )
  {
    try
    {
      FILE* results_file = fopen(argv[3], "aw");
      fprintf(results_file, "%s %s %s %s %s %s %s %s %s\n", "min_dist", "max_dist", "min_abs_dist", "max_abs_dist",
              "mean_dist", "mean_abs_dist", "rms_dist", "volume_overlap", "int_over_union");
      fprintf(results_file, "%f %f %f %f %f %f %f %f %f\n", diff.min_dist, diff.max_dist,
              diff.abs_min_dist, diff.abs_max_dist, diff.mean_dist, diff.abs_mean_dist,
              diff.rms_dist, diff.volume_overlap, diff.int_union_ratio);
      fclose(results_file);
    }
    catch(...)
    {
      std::cerr << "ERROR opening file to write results. Filename requested: " << argv[3] << std::endl;
      return 2;
    }
  }
  
  return 0;
}