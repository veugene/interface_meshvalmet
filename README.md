# **Background** #

This code implements a wrapper interface to MeshValmet for comparing VTK meshes. A simple commandline tool is also provided for comparing meshes. Meshvalmet is included in the source code.


# **Using the wrapper** #

### CompareMeshes.h ###


To compare two MeshValmet mesh objects with MeshValmet, use GetMeshDifferences().
There are two forms of this function:

```
GetMeshDifferences(struct model* mesh1, struct model* mesh2)
```
```
GetMeshDifferences(boost::shared_ptr<model> mesh1, boost::shared_ptr<model> mesh2)
```


This function takes MeshValmet mesh objects (*model* in MeshValmet).


### VTK_to_MeshValmet.h ###

To convert a VTK mesh object to a MeshValmet mesh object, use VTK_to_MeshValmet(). This function is declared as:

```
boost::shared_ptr<model> VTK_to_MeshValmet( vtkPolyData* vtk_mesh )
```


# **Requirements** #

* ITK 4.[?]
* VTK 6.[?]
* libproj
* C++ boost library

On Ubuntu 16.04, tested with libinsighttoolkit4.9, libinsighttoolkit4-dev, libvtk6.2, libvtk6-dev, libproj9, libproj-dev (4.9.2-2).


# **To build** #

* Create a build directory.
* Inside the build directory, run "cmake <path>" with <path> being the path to the source directory.
* Run "make"

# **Commandline tools** #

### example ###

This is example code that uses the MeshValmet wrapper. It assumes the two provided sample vtk files are in the folder from which it is run. Input files:

* sample_mesh_gt.vtk
* sample_mesh_seg.vtk

 The expected stdout output is:

```
OVERALL_DICE:0.868946
OVERALL_INT/UNION:0.768262
MIN DIST: -4.71815
MAX DIST: 5.28793
MIN ABS DIST: 0
MAX ABS DIST: 5.28793
MEAN DIST: 0.15221
MEAN ABS DIST: 1.22735
RMS DIST: 1.54404
VOLUME OVERLAP: 0.868946
INTERSECTION/UNION: 0.768262
```

### compare_meshes ###

Compare two meshes, each from an input file. Input files must either be "vtk" or "vtp" mesh format or some ITK-readable image format which is interpreted as a binary mask and internally converted into a mesh. The output is to stdout or appended to a text file. Usage:

```
./compare_meshes <mesh/image filename> <ground truth mesh/image filename> [<results file>]
```
