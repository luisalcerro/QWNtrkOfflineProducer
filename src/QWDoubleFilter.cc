#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class QWDoubleFilter : public edm::EDFilter {
public:
	explicit QWDoubleFilter(const edm::ParameterSet&);
	~QWDoubleFilter() {return;}
private:
	virtual bool filter(edm::Event&, const edm::EventSetup&);

	edm::InputTag	src_;
	double min_;
	double max_;
};

QWDoubleFilter::QWDoubleFilter(const edm::ParameterSet& pset) :
	src_(pset.getUntrackedParameter<edm::InputTag>("src")),
	min_(pset.getUntrackedParameter<double>("min")),
	max_(pset.getUntrackedParameter<double>("max"))
{
	consumes<double>(src_);
	return;
}

bool QWDoubleFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	Handle<double> psrc;
	iEvent.getByLabel( src_, psrc );

	double d = *psrc;
	if ( d > max_ or d < min_ ) return false;
	else return true;
}
