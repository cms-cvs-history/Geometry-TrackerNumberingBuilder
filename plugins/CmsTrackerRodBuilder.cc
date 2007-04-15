#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerRodBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsDetConstruction.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>


void CmsTrackerRodBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){
  CmsDetConstruction theCmsDetConstruction;
  theCmsDetConstruction.buildComponent(fv,g,s);  
}

void CmsTrackerRodBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
  std::vector< GeometricDet*> comp = det->components();

  std::stable_sort(comp.begin(),comp.end(),LessModZ()); 	

  if(comp.size()){
    for(uint32_t i=0; i<comp.size();i++){
      comp[i]->setGeographicalID(DetId(i+1));
    }
    
    det->deleteComponents();
    det->addComponents(comp);
  }else{
   edm::LogError("CmsTrackerRodBuilder")<<"Where are the Rod's modules?";
  }
}
