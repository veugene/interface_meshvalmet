#include "CompareMeshes.h"

// MeshValmet
#include "3dmodel.h"
#include "compute_error.h"
#include "compute_volume_overlap.h"
#include "geomutils.h"

CompareMeshes::mesh_differences CompareMeshes::GetMeshDifferences(struct model* mesh1, struct model* mesh2)
{
  CompareMeshes::mesh_differences results;
  memset(&results,0,sizeof(results));
  
  compute_distances( mesh1, mesh2, results );
  compute_overlap( mesh1, mesh2, results );
  
  return results;
}


void CompareMeshes::compute_distances(struct model* mesh1, struct model* mesh2, struct mesh_differences& diff)
{
  // Compute distances in from mesh1 to mesh2
  double bbox2_diag = dist_v(&mesh2->bBox[0], &mesh2->bBox[1]);
  sampling_step = 0.005*bbox2_diag;
  sampling_dens = 1.0/(sampling_step*sampling_step);
  min_sample_freq = 2;
  
  struct model_error* mesh1_err = (struct model_error*)malloc(sizeof(struct model_error));
  memset(mesh1_err,0,sizeof(*mesh1_err));
  mesh1_err->mesh = mesh1;
  
  struct dist_surf_surf_stats* stats = (struct dist_surf_surf_stats*)malloc(sizeof(struct dist_surf_surf_stats));
  memset(stats,0,sizeof(*stats));
  
  dist_surf_surf(mesh1_err, mesh2, sampling_dens, min_sample_freq, stats, 1, 0);
  
  
  // Compute distances in from mesh2 to mesh1
  double bbox1_diag = dist_v(&mesh1->bBox[0], &mesh1->bBox[1]);
  sampling_step = 0.005*bbox1_diag;
  sampling_dens = 1.0/(sampling_step*sampling_step);
  
  struct model_error* mesh2_err = (struct model_error*)malloc(sizeof(struct model_error));
  memset(mesh2_err,0,sizeof(*mesh2_err));
  mesh2_err->mesh = mesh2;
  
  struct dist_surf_surf_stats* stats_rev = (struct dist_surf_surf_stats*)malloc(sizeof(struct dist_surf_surf_stats));
  memset(stats_rev,0,sizeof(*stats_rev));
  
  dist_surf_surf(mesh2_err, mesh1, sampling_dens, min_sample_freq, stats_rev, 1, 0);
  
  
  // Summarize stats symmetrically
  diff.min_dist = min(stats->min_dist, stats_rev->min_dist);
  diff.max_dist = max(stats->max_dist, stats_rev->max_dist);
  diff.abs_min_dist = min(stats->abs_min_dist, stats_rev->abs_min_dist);
  diff.abs_max_dist = max(stats->abs_max_dist, stats_rev->abs_max_dist);
  diff.mean_dist = (stats->mean_dist + stats_rev->mean_dist)/2.0;
  diff.abs_mean_dist = (stats->abs_mean_dist*stats->m1_samples + stats_rev->abs_mean_dist*stats_rev->m1_samples)/(stats->m1_samples+stats_rev->m1_samples);
  diff.rms_dist = sqrt(
                        ( stats->m1_samples*(stats->rms_dist*stats->rms_dist) + stats_rev->m1_samples*(stats_rev->rms_dist*stats_rev->rms_dist) )
                        / (stats->m1_samples+stats_rev->m1_samples)
                      );
}


// Taken from MeshValmetControls.cxx
void CompareMeshes::compute_overlap(struct model* mesh1, struct model* mesh2, struct mesh_differences& diff)
{
  int num_vert1 = mesh1->num_vert;
  int num_vert2 = mesh2->num_vert;
  int num_faces1 = mesh1->num_faces;
  int num_faces2 = mesh2->num_faces;
  
  double * L1 = new double[3*num_vert1];
  double * L2 = new double[3*num_vert2];
  int * T1 = new int[3*num_faces1];
  int * T2 = new int[3*num_faces2];
  
  vertex_t * vert_list = mesh1->vertices;
  for(int i=0; i<num_vert1; i++) {
    L1[3*i+0] = vert_list[i].x;
    L1[3*i+1] = vert_list[i].y;
    L1[3*i+2] = vert_list[i].z;
  }
  vert_list = mesh2->vertices;
  for(int i=0; i<num_vert2; i++) {
    L2[3*i+0] = vert_list[i].x;
    L2[3*i+1] = vert_list[i].y;
    L2[3*i+2] = vert_list[i].z;
  }
  
  face_t * face_list = mesh1->faces;
  for(int i=0; i<num_faces1; i++) {
    T1[3*i+0] = face_list[i].f0+1;
    T1[3*i+1] = face_list[i].f1+1;
    T1[3*i+2] = face_list[i].f2+1;
  }
  face_list = mesh2->faces;
  for(int i=0; i<num_faces2; i++) {
    T2[3*i+0] = face_list[i].f0+1;
    T2[3*i+1] = face_list[i].f1+1;
    T2[3*i+2] = face_list[i].f2+1;
  } 
  
  ComputeRobustVolumeOverlap(L1,L2,num_vert1,num_vert2,T1,T2,num_faces1,num_faces2,&(diff.volume_overlap),&(diff.int_union_ratio));
  
  delete [] L1;
  delete [] L2;
  delete [] T1;
  delete [] T2;
}