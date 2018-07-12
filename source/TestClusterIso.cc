/*!
 * \file TestClusterIso.cc
 * \brief 
 * \author Francesco Vassalli <Francesco.Vassalli@colorado.edu> 
 * \author Chase Smith <chsm5267@colorado.edu>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "TestClusterIso.h"
#include <iostream>

#include <fun4all/SubsysReco.h>

#include <phool/getClass.h>         
#include <phool/PHCompositeNode.h>  

#include <calobase/RawCluster.h>          
#include <calobase/RawClusterUtility.h>   
#include <calobase/RawClusterContainer.h> 

#include <calobase/RawTower.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h> 
#include <calobase/RawTowerGeomContainer.h>  

#include <g4vertex/GlobalVertex.h>    
#include <g4vertex/GlobalVertexMap.h>

/** \Brief Function to get correct tower eta
 *
 * Each tower is calculated using the vertex (0,0,0)
 * which is incorrect in many collisions. This function 
 * uses geometry to find eta using correct vertex.
 */
double TestClusterIso::getTowerEta(RawTowerGeom* tower_geom, double vx, double vy, double vz) 
{
  if(vx==0&&vy==0&&vz==0){
    return tower_geom->get_eta();
  }
  else{
   double r= sqrt((tower_geom->get_center_x()-vx)*(tower_geom->get_center_x()-vx)+(tower_geom->get_center_y()-vy)*(tower_geom->get_center_y()-vy));
   double theta = atan2(r,tower_geom->get_center_z()-vz);
   return -log(tan(theta/2.));
  }
}


TestClusterIso::ClusterIso(const std::string &kname, float m_eTCut, float m_coneSize, bool m_eventSubtraction) : SubsysReco("TestClusterIso"), m_eTCut(m_eTCut), m_coneSize(m_coneSize), m_eventSubtraction(m_eventSubtraction){
  std::cout<<"Begining Cluster Isolation Energy Calculation"<<'\n';
  m_vx=m_vy=m_vz=0;
}

 int TestClusterIso::Init(PHCompositeNode *topNode)
{
  return 0;
}

void TestClusterIso::seteTCut(float eTCut){
  this->m_eTCut = eTCut;
}

void TestClusterIso::setConeSize(float coneSize){
  this->m_coneSize=coneSize;
}
void TestClusterIso::setEventSubtraction(bool eventSubtraction){
  this->m_eventSubtraction=eventSubtraction;
}

const float TestClusterIso::geteTCut(){
  return m_eTCut;
}

const float TestClusterIso::getConeSize(){
  return m_coneSize;
}

const float TestClusterIso::isSubtracted(){
  return m_eventSubtraction;
}

/**
 * Must be called to set the new vertex for the cluster 
 */
const CLHEP::Hep3Vector TestClusterIso::getVertex(){
  return CLHEP::Hep3Vector( m_vx, m_vy, m_vz);
}

/** \Brief process_event is where isolation Energy is calculated for all clusters
 *
 * For each cluster in the EMCal go through all of the towers in each calorimeter, 
 * if the towers are within the iso cone add their energy to the sum. Finally 
 * subtract the cluster energy from the sum 
 */
 int TestClusterIso::process_event(PHCompositeNode *topNode)
{
  /**
   * If there event is embedded in Au+Au or another larger background we want to 
   * get isolation energy from the towers with a subtracted background. This boolean
   * looks at those towers instead of the original objects which include the background.
   * NOTE: that during the background event subtraction the EMCal towers are grouped 
   * together so we have to use the inner HCal geometry. 
   */
  if(m_eventSubtraction)
  {
    ///get EMCal towers
    RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
    std::cout << "TestClusterIso::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC_RETOWER_SUB1 towers" << '\n';

    ///get InnerHCal towers
    RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
    std::cout << "TestClusterIso::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN_SUB1 towers" << '\n';

    ///get outerHCal towers
    RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
    std::cout << "TestClusterIso::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;

    ///get geometry of calorimeter towers
    RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
    RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
    RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

    {
      RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
      RawClusterContainer::ConstRange begin_end = clusters->getClusters();
      RawClusterContainer::ConstIterator rtiter;
      std::cout << " TestClusterIso sees " << clusters->size() << " clusters " << '\n';
      
      ///vertexmap is used to get correct collision vertex
      GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap"); 
      m_vx=m_vy=m_vz=0;
      if (vertexmap&&!vertexmap->empty())
      {
         GlobalVertex* vertex = (vertexmap->begin()->second);
         m_vx = vertex->get_x();
         m_vy = vertex->get_y();
         m_vz = vertex->get_z();
         std::cout<<"Event Vertex Calculated in TestClusterIso x:"<<m_vx<<" y:"<<m_vy<<" z:"<<m_vz<<'\n';
      }

      for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {

        RawCluster *cluster = rtiter->second;
        
        CLHEP::Hep3Vector vertex( m_vx, m_vy, m_vz); 
        CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
        double cluster_energy = E_vec_cluster.mag();
        double cluster_eta = E_vec_cluster.pseudoRapidity(); 
        double cluster_phi = E_vec_cluster.phi();
        double et = cluster_energy / cosh( cluster_eta );
        double isoEt=0;

        if (et < m_eTCut){continue;} //continue if cluster is under eT cut

        ///calculate EMCal tower contribution to isolation energy
        {
          RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
            RawTower *tower = rtiter->second; 
            RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
            double this_phi = tower_geom->get_phi();
            double this_eta= tower_geom->get_eta();
            if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize){
              isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
            }
          }
        }

        ///calculate Inner HCal tower contribution to isolation energy
        {
          RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
            RawTower *tower = rtiter->second; 
            RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
            double this_phi = tower_geom->get_phi();
            double this_eta= getTowerEta(tower_geom,m_vx,m_vy,m_vz);
            if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize){
              isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
            }
          }
        }

        ///calculate Outer HCal tower contribution to isolation energy
        {
          RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
            RawTower *tower = rtiter->second; 
            RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());
            double this_phi = tower_geom->get_phi();
            double this_eta= getTowerEta(tower_geom,m_vx,m_vy,m_vz); 
            if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize){
              isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
            }
          }
        }

        isoEt-=et; //Subtract cluster eT from isoET
        cluster->set_et_iso(isoEt);
      }
    }
  }

  /**
   * If the event is not embedded in any kind of background we just use the original 
   * tower objects to get isolation energy.
   */
  else
  {
    ///get EMCal towers
    RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
    std::cout << "TestClusterIso::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC towers" << '\n';

    ///get InnerHCal towers
    RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
    std::cout << "TestClusterIso::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << '\n';

    ///get outerHCal towers
    RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
    std::cout << "TestClusterIso::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;

    ///get geometry of calorimeter towers
    RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
    RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
    RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

    {
      RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
      RawClusterContainer::ConstRange begin_end = clusters->getClusters();
      RawClusterContainer::ConstIterator rtiter;
      std::cout << " TestClusterIso sees " << clusters->size() << " clusters " << '\n';
      
      ///vertexmap is used to get correct collision vertex
      GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap"); 
      m_vx=m_vy=m_vz=0;
      if (vertexmap&&!vertexmap->empty())
      {
         GlobalVertex* vertex = (vertexmap->begin()->second);
         m_vx = vertex->get_x();
         m_vy = vertex->get_y();
         m_vz = vertex->get_z();
         std::cout<<"Event Vertex Calculated in TestClusterIso x:"<<m_vx<<" y:"<<m_vy<<" z:"<<m_vz<<'\n';
      }

      for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {

        RawCluster *cluster = rtiter->second;
        
        CLHEP::Hep3Vector vertex( m_vx, m_vy, m_vz); 
        CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*cluster, vertex);
        double cluster_energy = E_vec_cluster.mag();
        double cluster_eta = E_vec_cluster.pseudoRapidity(); 
        double cluster_phi = E_vec_cluster.phi();
        double et = cluster_energy / cosh( cluster_eta );
        double isoEt=0;

        if (et < m_eTCut){continue;} //continue if cluster is under eT cut

        ///calculate EMCal tower contribution to isolation energy
        {
          RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
            RawTower *tower = rtiter->second; 
            RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
            double this_phi = tower_geom->get_phi();
            double this_eta= getTowerEta(tower_geom,m_vx,m_vy,m_vz);
            if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize){
              isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
            }
          }
        }

        ///calculate Inner HCal tower contribution to isolation energy
        {
          RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
            RawTower *tower = rtiter->second; 
            RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
            double this_phi = tower_geom->get_phi();
            double this_eta= getTowerEta(tower_geom,m_vx,m_vy,m_vz);
            if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize){
              isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
            }
          }
        }

        ///calculate Outer HCal tower contribution to isolation energy
        {
          RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
            RawTower *tower = rtiter->second; 
            RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());
            double this_phi = tower_geom->get_phi();
            double this_eta= getTowerEta(tower_geom,m_vx,m_vy,m_vz); 
            if(deltaR( cluster_eta, this_eta, cluster_phi, this_phi ) < m_coneSize){
              isoEt += tower->get_energy() / cosh( this_eta ); //if tower is in cone, add energy
            }
          }
        }

        isoEt-=et; //Subtract cluster eT from isoET
        cluster->set_et_iso(isoEt);
      }
    }
  }
  return 0;
}



 int TestClusterIso::End(PHCompositeNode *topNode)
{
  return 0;
}
