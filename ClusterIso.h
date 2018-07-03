
// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTower.h>
#include "TMath.h"
class PHCompositeNode;

class ClusterIso: public SubsysReco
{

 public:

  ClusterIso(const std::string& ,float& pTCut, float& coneSize);

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);
  void setpTCut(float&);
  void setConeSize(float&);
  float getpTCut(float&);
  float getConeSize(float&);

 private:
  float _b_vx;
  float _b_vy;
  float _b_vz;

  float deltaR( float eta1, float eta2, float phi1, float phi2 ) {

    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;

    return TMath::Pow(( deta*deta + dphi*dphi),.5);

  }
  bool towerInCluster(RawCluster* cluster, RawTower* tower){
    return cluster->get_towermap().cend() != cluster->get_towermap().find(tower->get_key());
  }

  float pTCut;
  float coneSize;

};