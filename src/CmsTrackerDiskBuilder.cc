#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerDiskBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/interface/ExtractStringFromDDD.h"
#include "Geometry/TrackerNumberingBuilder/interface/CmsTrackerPanelBuilder.h"
#include "Geometry/TrackerNumberingBuilder/interface/TrackerStablePhiSort.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>


void CmsTrackerDiskBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){

  CmsTrackerPanelBuilder   theCmsTrackerPanelBuilder;
  GeometricDet * subdet = new GeometricDet(&fv,theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv)));

  switch (theCmsTrackerStringToEnum.type(ExtractStringFromDDD::getString(s,&fv))){
  case GeometricDet::panel:
    theCmsTrackerPanelBuilder.build(fv,subdet,s);
    break;
  default:
    edm::LogError("CmsTrackerDiskBuilder")<<" ERROR - I was expecting a Panel, I got a "<<ExtractStringFromDDD::getString(s,&fv);   
  }  
  g->addComponent(subdet);
}

void CmsTrackerDiskBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){

 std::vector< GeometricDet* > comp = det->components();

 switch(det->components().front()->type()){
 case GeometricDet::panel:
   TrackerStablePhiSort(comp.begin(),comp.end(), ExtractPhi());
   break;
 default:
   edm::LogError("CmsTrackerDiskBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type();
 }

  std::vector< GeometricDet* > zminpanels;  // Here z refers abs(z);
  std::vector< GeometricDet* > zmaxpanels;   // So, zmin panel is always closer to ip.
  zminpanels.clear();
  zmaxpanels.clear();

  uint32_t  totalblade = comp.size()/2;
  if ( totalblade != 24 )
        edm::LogError("CmsTrackerDiskBuilder")<<"ERROR, The Total Number of Blade in one disk is "<<totalblade;

  for( uint32_t  j=0; j<totalblade ;j++)
  {
    if ( fabs( comp[2*j]->translation().z() ) > fabs( comp[ 2*j +1 ]->translation().z() ) ) {
         zmaxpanels.push_back(comp[2*j] );
         zminpanels.push_back(comp[2*j+1]);

    }else if ( fabs( comp[2*j]->translation().z() ) < fabs( comp[ 2*j +1 ]->translation().z() ) ) {
         zmaxpanels.push_back(comp[2*j+1] );
         zminpanels.push_back(comp[2*j]);

    }else {
       edm::LogWarning("CmsTrackerDiskBuilder")<<"WARNING - The Z of  both panels are equal! ";
    }

  }

  for ( uint32_t  fn=0; fn< zminpanels.size(); fn++) {
    uint32_t blade = fn+1;
    uint32_t panel = 1;
    uint32_t temp = (blade<<2) | panel;
    zminpanels[fn]->setGeographicalID(DetId(temp));
  }
  
  for (uint32_t  bn=0; bn< zmaxpanels.size(); bn++) {
    uint32_t blade = bn+1;
    uint32_t panel = 2;
    uint32_t temp = (blade<<2) | panel;
    zmaxpanels[bn]->setGeographicalID(DetId(temp));
  }
  
  det->deleteComponents();
  
  det->addComponents(zminpanels);
  det->addComponents(zmaxpanels);
}