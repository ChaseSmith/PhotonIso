#ifndef ClusterIso_h 
#define ClusterIso_h

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTower.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <cmath>
class PHCompositeNode;


class ClusterIso: public SubsysReco
{

public:

  ClusterIso(const std::string& ,float eTCut, float coneSize);

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);
  void seteTCut(float x);
  void setConeSize(float x);
  float geteTCut();
  float getConeSize();


protected:
  float deltaR( float eta1, float eta2, float phi1, float phi2 ) {

    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;

    return sqrt( deta*deta + dphi*dphi);

  }
  inline bool towerInCluster(RawCluster* cluster, RawTower* tower){
    return cluster->get_towermap().cend() != cluster->get_towermap().find(tower->get_key());
  }
  inline double getTowerEta(RawTowerGeom* tower_geom, double vx, double vy, double vz) // need to transpose the eta 
  {
    if(vx==0&&vy==0&&vz==0){
      return tower_geom->get_eta();
    }
    else{
     double r= tower_geom->get_center_radius();
     double x = r*cos(tower_geom->get_phi())-vx;
     double y = r*sin(tower_geom->get_phi())-vy;
     double z = r/tan(2*atan2(exp(-1*tower_geom->get_eta()),1))-vz;
     r= sqrt(x*x+y*y);
     return -log(tan(atan2(r,z)/2.));
    }
  }

  float eTCut;
  float coneSize;

private:
  float vx;
  float vy;
  float vz;

};
#endif //ClusterIso_h
