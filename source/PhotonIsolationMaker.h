#ifndef __TRUTHJETTRIGGER_H__
#define __TRUTHJETTRIGGER_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

#include "TTree.h"
#include "TFile.h"

class PHCompositeNode;

class PhotonIsolationMaker: public SubsysReco
{

 public:

  PhotonIsolationMaker(const std::string &name="PhotonIsolationMaker.root");

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:

  float deltaR( float eta1, float eta2, float phi1, float phi2 ) {

    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;

    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );

  }

  TFile *_f;

  TTree *_tree;

  std::string _foutname;

  int _b_cluster_n;
  float _b_cluster_pt[500];
  float _b_cluster_eta[500];
  float _b_cluster_phi[500];

  int _b_particle_n;
  float _b_particle_pt[1000];
  float _b_particle_eta[1000];
  float _b_particle_phi[1000];
  int _b_particle_pid[1000];
  float _b_particle_calo_iso_0[1000];
  float _b_particle_calo_iso_1[1000];
  float _b_particle_calo_iso_2[1000];

};

#endif // __TRUTHJETTRIGGER_H__
