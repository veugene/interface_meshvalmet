#ifndef vtkPLYStreamWriter_h
#define vtkPLYStreamWriter_h

#include "vtkIOPLYModule.h" // For export macro
#include "vtkWriter.h"
#include "vtkPLY.h"
#include <stdio.h>

class vtkScalarsToColors;
class vtkDataSetAttributes;

#define VTK_LITTLE_ENDIAN 0
#define VTK_BIG_ENDIAN    1

#define VTK_COLOR_MODE_DEFAULT 0
#define VTK_COLOR_MODE_UNIFORM_CELL_COLOR 1
#define VTK_COLOR_MODE_UNIFORM_POINT_COLOR 2
#define VTK_COLOR_MODE_UNIFORM_COLOR 3
#define VTK_COLOR_MODE_OFF 4

class vtkPolyData;

class VTKIOPLY_EXPORT vtkPLYStreamWriter : public vtkWriter
{
public:
  static vtkPLYStreamWriter *New();
  vtkTypeMacro(vtkPLYStreamWriter,vtkWriter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // If the file type is binary, then the user can specify which
  // byte order to use (little versus big endian).
  vtkSetClampMacro(DataByteOrder,int,VTK_LITTLE_ENDIAN,VTK_BIG_ENDIAN);
  vtkGetMacro(DataByteOrder,int);
  void SetDataByteOrderToBigEndian()
  {this->SetDataByteOrder(VTK_BIG_ENDIAN);}
  void SetDataByteOrderToLittleEndian()
  {this->SetDataByteOrder(VTK_LITTLE_ENDIAN);}
  
  // Description:
  // These methods enable the user to control how to add color into the PLY
  // output file. The default behavior is as follows. The user provides the
  // name of an array and a component number. If the type of the array is
  // three components, unsigned char, then the data is written as three
  // separate "red", "green" and "blue" properties. If the type is not
  // unsigned char, and a lookup table is provided, then the array/component
  // are mapped through the table to generate three separate "red", "green"
  // and "blue" properties in the PLY file. The user can also set the
  // ColorMode to specify a uniform color for the whole part (on a vertex
  // colors, face colors, or both. (Note: vertex colors or cell colors may be
  // written, depending on where the named array is found. If points and
  // cells have the arrays with the same name, then both colors will be
  // written.)
  vtkSetMacro(ColorMode,int);
  vtkGetMacro(ColorMode,int);
  void SetColorModeToDefault()
  {this->SetColorMode(VTK_COLOR_MODE_DEFAULT);}
  void SetColorModeToUniformCellColor()
  {this->SetColorMode(VTK_COLOR_MODE_UNIFORM_CELL_COLOR);}
  void SetColorModeToUniformPointColor()
  {this->SetColorMode(VTK_COLOR_MODE_UNIFORM_POINT_COLOR);}
  void SetColorModeToUniformColor() //both cells and points are colored
  {this->SetColorMode(VTK_COLOR_MODE_UNIFORM_COLOR);}
  void SetColorModeToOff() //No color information is written
  {this->SetColorMode(VTK_COLOR_MODE_OFF);}
  
  // Description:
  // Specify the array name to use to color the data.
  vtkSetStringMacro(ArrayName);
  vtkGetStringMacro(ArrayName);
  
  // Description:
  // Specify the array component to use to color the data.
  vtkSetClampMacro(Component,int,0,VTK_INT_MAX);
  vtkGetMacro(Component,int);
  
  // Description:
  // A lookup table can be specified in order to convert data arrays to using fdopen
  // RGBA colors.
  virtual void SetLookupTable(vtkScalarsToColors*);
  vtkGetObjectMacro(LookupTable,vtkScalarsToColors);
  
  // Description:
  // Set the color to use when using a uniform color (either point or cells,
  // or both). The color is specified as a triplet of three unsigned chars
  // between (0,255). This only takes effect when the ColorMode is set to
  // uniform point, uniform cell, or uniform color.
  vtkSetVector3Macro(Color,unsigned char);
  vtkGetVector3Macro(Color,unsigned char);
  
  // Description:
  // Get the input to this writer.
  vtkPolyData* GetInput();
  vtkPolyData* GetInput(int port);
  
  // Description:
  // Specify file name of vtk polygon data file to write.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  
  // Description:
  // Specify file type (ASCII or BINARY) for vtk data file.
  vtkSetClampMacro(FileType,int,VTK_ASCII,VTK_BINARY);
  vtkGetMacro(FileType,int);
  void SetFileTypeToASCII() {this->SetFileType(VTK_ASCII);};
  void SetFileTypeToBinary() {this->SetFileType(VTK_BINARY);};
  
  // Description:
  // Return the written data block; the size of the data block
  char* getWrittenDataBlock() {return data_block;};
  size_t getWrittenDataBlockSize() {return data_block_size;};
  
protected:
  vtkPLYStreamWriter();
  ~vtkPLYStreamWriter();
  
  void WriteData();
  unsigned char *GetColors(vtkIdType num, vtkDataSetAttributes *dsa);
  
  PlyFile* ply_open_for_writing(int nelems, const char **elem_names, int file_type, float *version);
  char* data_block;
  size_t data_block_size;
  
  int DataByteOrder;
  char *ArrayName;
  int Component;
  int ColorMode;
  vtkScalarsToColors *LookupTable;
  unsigned char Color[3];
  
  char* FileName;
  
  int FileType;
  
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  
private:
  vtkPLYStreamWriter(const vtkPLYStreamWriter&);  // Not implemented.
  void operator=(const vtkPLYStreamWriter&);  // Not implemented.
  
  void setFileName(const char* filename);    // Disabled
  char* getFileName();                       // Disabled
};

#endif // vtkPLYStreamWriter_h