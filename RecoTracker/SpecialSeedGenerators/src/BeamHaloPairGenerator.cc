#include "RecoTracker/SpecialSeedGenerators/interface/BeamHaloPairGenerator.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingLayerSetsBuilder.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
using namespace ctfseeding;


BeamHaloPairGenerator::BeamHaloPairGenerator(const edm::ParameterSet& conf): conf_(conf){
	edm::LogInfo("CtfSpecialSeedGenerator|BeamHaloPairGenerator") << "Constructing BeamHaloPairGenerator";
	theMaxTheta=conf.getParameter<double>("maxTheta");
} 


SeedingLayerSets BeamHaloPairGenerator::init(const edm::EventSetup& es){
	edm::ParameterSet leyerPSet = conf_.getParameter<edm::ParameterSet>("LayerPSet");
	SeedingLayerSetsBuilder lsBuilder(leyerPSet);
  	SeedingLayerSets lss = lsBuilder.layers(es);
	return lss;	
}


const OrderedSeedingHits& BeamHaloPairGenerator::run(const TrackingRegion& region,
                          			    const edm::Event& e,
                              			    const edm::EventSetup& es){
	hitPairs.clear();
	SeedingLayerSets lss = init(es);
	SeedingLayerSets::const_iterator iLss;
	for (iLss = lss.begin(); iLss != lss.end(); iLss++){
		SeedingLayers ls = *iLss;
		if (ls.size() != 2){
                	throw cms::Exception("CtfSpecialSeedGenerator") << "You are using " << ls.size() <<" layers in set instead of 2 ";
        	}	
		std::vector<SeedingHit> innerHits  = region.hits(e, es, &ls[0]);
		std::vector<SeedingHit> outerHits  = region.hits(e, es, &ls[1]);
		std::vector<SeedingHit>::const_iterator iOuterHit;
		for (iOuterHit = outerHits.begin(); iOuterHit != outerHits.end(); iOuterHit++){
			std::vector<SeedingHit>::const_iterator iInnerHit;
			for (iInnerHit = innerHits.begin(); iInnerHit != innerHits.end(); iInnerHit++){
			  //do something in there... if necessary
			  const TransientTrackingRecHit::ConstRecHitPointer & crhpi = *iInnerHit;
			  const TransientTrackingRecHit::ConstRecHitPointer & crhpo =  *iOuterHit;
			  GlobalVector d=crhpo->globalPosition() - crhpi->globalPosition();
			  if (fabs(d.theta())>theMaxTheta) {;continue;}


			  hitPairs.push_back(OrderedHitPair(*iInnerHit,
							    *iOuterHit));
			}
		}
        }
	return hitPairs;
}
