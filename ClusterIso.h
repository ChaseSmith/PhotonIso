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
    float newVertexEta(int towerType , float oldEta, float vx, float vy, float vz)
  {
    float radiusToTower; 
    switch(towerType)
    {
      case 0:
        radiusToTower = 90.0; //900mm, sPHENIX EMCal 
        break;
      case 1:
        radiusToTower = 115.7; //1157mm, sPHENIX IHCal 
        break;
      case 2:
        radiusToTower = 182.0; //1820mm, sPHENIX OHCal 
        break;
      default:
        std::cout<<"Did not select a valid tower towerType"<<std::endl;
        return -99;
    }
    float newEta;
    float distAlongZ;
    float newDistAlongZ;
    if(oldEta >= 0)
    {
      distAlongZ = fabs(radiusToTower/tan(2*atan(exp(-1*oldEta))));
      newDistAlongZ = distAlongZ - vz;
      newEta = -1*log(tan(0.5*atan(radiusToTower/newDistAlongZ)));
    }
    else if (oldEta < 0)
    {
      distAlongZ = fabs(radiusToTower/tan(2*atan(exp(-1*oldEta)))); //note this should be negative but isnt for the log
      newDistAlongZ = distAlongZ + vz;
      newEta = log(tan(0.5*atan(radiusToTower/newDistAlongZ)));
    }

    return newEta;
  }
  inline double getTowerEta(RawTowerGeom* tower_geom, double vx, double vy, double vz) // need to transpose the eta 
  {
    double r= tower_geom.get_center_radius();
    double x = r*cos(tower_geom->get_phi())-vx;
    double y = r*sin(tower_geom->get_phi())-vy;
    double z = r/tan(2*atan2(exp(-1*tower_geom->get_eta()),1))-vz;
    r= sqrt(x*x+y*y);
    return -log(tan(atan2(r,z)/2.));
  }

  float eTCut;
  float coneSize;

private:
  float vx;
  float vy;
  float vz;

};
#endif //ClusterIso_h
