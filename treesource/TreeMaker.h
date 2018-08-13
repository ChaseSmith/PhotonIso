#ifndef TreeMaker_h 
#define TreeMaker_h

#include <fun4all/SubsysReco.h>
#include <vector>
#include "TTree.h"
#include "TFile.h"
#include <string>

class PHCompositeNode;

class ChaseTower
{
public:
  //constructors
  ChaseTower(){}; 
  ChaseTower(double eta_in, double phi_in, double energy_in, unsigned int keytype_in)
  {
    eta = eta_in;
    phi = phi_in;
    energy = energy_in;
    keytype = keytype_in;
  }
  void setEta(double eta2){eta = eta2;}
  void setPhi(double phi2){phi = phi2;}
  void setEnergy(double energy2){energy = energy2;}
  void setKey(unsigned int keytype2){keytype = keytype2;}
  double getEta(){return eta;}
  double getPhi(){return phi;}
  double getEnergy(){return energy;}
  unsigned int getKey(){return keytype;}
private:
  double eta;
  double phi;
  double energy;
  unsigned int keytype;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class TreeMaker: public SubsysReco
{

 public:

  TreeMaker(const std::string &name="TreeMaker.root", int embed_id = 0);

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

 private:

  float deltaR( float eta1, float eta2, float phi1, float phi2 ) 
  {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
  }

  TFile *_f;

  TTree *_tree;

  std::string _foutname;

  float vx;
  float vy;
  float vz;

  int _embed_id;

  int _b_cluster_n;
  double _b_cluster_et[500];
  double _b_cluster_eta[500];
  double _b_cluster_phi[500];
  double _b_cluster_prob[500];
  double _b_et_iso_calotower_sub_R01[500];
  double _b_et_iso_calotower_R01[500];
  double _b_et_iso_calotower_sub_R02[500];
  double _b_et_iso_calotower_R02[500];
  double _b_et_iso_calotower_sub_R03[500];
  double _b_et_iso_calotower_R03[500];
  double _b_et_iso_calotower_sub_R04[500];
  double _b_et_iso_calotower_R04[500];

  int _b_NTowers[500];
  double _b_etot[500];

  int _b_particle_n;
  float _b_particle_pt[1000];
  float _b_particle_et[1000];
  float _b_particle_eta[1000];
  float _b_particle_phi[1000];
  int _b_particle_pid[1000];


};
#endif


