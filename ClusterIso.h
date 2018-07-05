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

  ClusterIso(const std::string& ,float pTCut, float coneSize);

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);
  void setpTCut(float x);
  void setConeSize(float x);
  float getpTCut();
  float getConeSize();


protected:
  float deltaR( float eta1, float eta2, float phi1, float phi2 ) {

    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
    if ( dphi < -3.14159 ) dphi += 2 * 3.14159;

    return sqrt( deta*deta + dphi*dphi);

  }
  bool towerInCluster(RawCluster* cluster, RawTower* tower){
    return cluster->get_towermap().cend() != cluster->get_towermap().find(tower->get_key());
  }
  double getTowerEta(RawTowerGeom tower_geom, double vx, double vy, double vz) // need to transpose the eta 
  {
    double radius = sqrt((tower_geom.get_center_x()-vx)*(tower_geom.get_center_x()-vx)+(tower_geom.get_center_y()-vy)*(tower_geom.get_center_y()-vy));
    double theta = atan2(radius,tower_geom.get_center_z()-vz);
    return -log(tan(theta/2.));
  }

  float pTCut;
  float coneSize;

private:
  float vx;
  float vy;
  float vz;

};
#endif //ClusterIso_h