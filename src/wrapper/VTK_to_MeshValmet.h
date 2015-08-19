#ifndef VTK_to_MeshValmet_h
#define VTK_to_MeshValmet_h

// Modified VTK
#include "vtkPLYStreamWriter.h"

// MeshValmet
#include "types.h"
#include "3dmodel.h"
#include "model_in.h"
#include "model_in_ply.h"
#include "geomutils.h"
#include "compute_error.h" 
#include "vtkSmartPointer.h"
#include "vtkPLYStreamWriter.h"
#include "vtkPolyData.h"

#define GZ_BUF_SZ   16384

model* VTK_to_MeshValmet( vtkPolyData* vtk_mesh )
{
  // Get the readable end of a pipe from the modified VTK PLY writer class and open it as a file
  vtkSmartPointer<vtkPLYStreamWriter> stream = vtkSmartPointer<vtkPLYStreamWriter>::New();
  stream->SetInputData(vtk_mesh);
  stream->SetFileTypeToASCII();
  stream->Write();
  char* data_block = stream->getWrittenDataBlock();
  size_t data_block_size = stream->getWrittenDataBlockSize();
  FILE* stream_file = fmemopen(data_block, data_block_size, "rb");  
  
  // Prepare the MeshValmet data struct which contains the open file pointer
  struct file_data* data;
  data = (struct file_data*)malloc(sizeof(struct file_data));
  data->f = stream_file;
  data->block = (unsigned char*)malloc(GZ_BUF_SZ*sizeof(unsigned char));
  data->size = GZ_BUF_SZ;
  data->eof_reached = 0;
  data->nbytes = 0;
  data->pos = 1;
  data->is_binary = 0;
  
  // Read the mesh data using MeshValmet
  model* mesh;
  mesh = (model*)malloc(sizeof(model));
  memset(mesh,0,sizeof(*mesh));
  read_ply_tmesh(&mesh, data);
  
  // Close the file and the pipe end
  fclose(stream_file);
  
  // Return the MeshValmet mesh object
  return mesh;
}

#endif // VTK_to_MeshValmet_h