#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/HiGenData/interface/GenHIEvent.h"

#include "TFile.h"
#include "TH2.h"
#include "TMath.h"

using namespace std;
class QWInt2Double : public edm::EDProducer {
public:
	explicit QWInt2Double(const edm::ParameterSet&);
	~QWInt2Double();

private:
	virtual void produce(edm::Event&, const edm::EventSetup&) override;
	///
	edm::InputTag	src_;
};

QWInt2Double::QWInt2Double(const edm::ParameterSet& pset) :
	src_(pset.getUntrackedParameter<edm::InputTag>("src"))
{
#if	CMSSW_VERSION > 600
	consumes<int>(src_);
#endif

	produces<double>();
}

QWInt2Double::~QWInt2Double()
{
	return;
}

void QWInt2Double::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	Handle<int> pX;
	iEvent.getByLabel(src_, pX);

        auto_ptr<double> p(new double(*pX));

	iEvent.put(p);
}


DEFINE_FWK_MODULE(QWInt2Double);
