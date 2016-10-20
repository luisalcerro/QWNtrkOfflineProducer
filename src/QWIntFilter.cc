// system include files
#include <memory>
#include <vector>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"

//
// class declaration
//

class QWIntFilter : public edm::EDFilter {
   public:
      explicit QWIntFilter(const edm::ParameterSet&);
      ~QWIntFilter();

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      // ----------member data ---------------------------

  std::vector<int> selectedBins_;
  edm::Handle<int> cbin_;
  edm::InputTag	tag_;


};

QWIntFilter::QWIntFilter(const edm::ParameterSet& iConfig):
  selectedBins_(iConfig.getParameter<std::vector<int> >("selectedBins")),
  tag_(iConfig.getParameter<edm::InputTag>("BinLabel"))
{

}


QWIntFilter::~QWIntFilter()
{
}

// ------------ method called on each new Event  ------------
bool
QWIntFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  bool result = false;

   using namespace edm;
   iEvent.getByLabel(tag_,cbin_);

   int bin = *cbin_;

   for(unsigned int i = 0; i < selectedBins_.size(); ++i){
     if(bin == selectedBins_[i]) result = true;
   }

   return result;
}

// ------------ method called once each job just before starting event loop  ------------
void 
QWIntFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
QWIntFilter::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(QWIntFilter);
