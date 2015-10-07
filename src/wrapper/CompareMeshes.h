#ifndef CompareMeshes_h
#define CompareMeshes_h

// MeshValmet
#include "3dmodel.h"

// Boost
#include <boost/shared_ptr.hpp>

class CompareMeshes
{
public:
  CompareMeshes() : sampling_step(1), sampling_dens(1), min_sample_freq(1) {};
  
  struct mesh_differences
  {
    double min_dist;
    double max_dist;
    double abs_min_dist;
    double abs_max_dist;
    double mean_dist;
    double abs_mean_dist;
    double rms_dist;
    double volume_overlap;
    double int_union_ratio;
  };
  
  mesh_differences GetMeshDifferences(struct model* mesh1, struct model* mesh2);
  mesh_differences GetMeshDifferences(boost::shared_ptr<model> mesh1, boost::shared_ptr<model> mesh2);
  
protected:
  void compute_distances(struct model* mesh1, struct model* mesh2, struct mesh_differences& diff);
  void compute_overlap(struct model* mesh1, struct model* mesh2, struct mesh_differences& diff);
  
  double sampling_step;
  double sampling_dens;
  double min_sample_freq;
};

#endif // CompareMeshes_h