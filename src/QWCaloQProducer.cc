#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"

//#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/TrackFwd.h"
//#include "DataFormats/VertexReco/interface/Vertex.h"
//#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include <complex>
#include <TMath.h>

class QWCaloQProducer : public edm::EDProducer {
public:
	explicit QWCaloQProducer(const edm::ParameterSet&);
	~QWCaloQProducer();

private:
	virtual void produce(edm::Event&, const edm::EventSetup&) override;
	edm::InputTag	caloSrc_;
	double		etaMin_;
	double		etaMax_;
	int		N_;
};

QWCaloQProducer::QWCaloQProducer(const edm::ParameterSet& pset) :
	caloSrc_(pset.getUntrackedParameter<edm::InputTag>("caloSrc")),
	etaMin_(pset.getUntrackedParameter<double>("etaMin")),
	etaMax_(pset.getUntrackedParameter<double>("etaMax")),
	N_(pset.getUntrackedParameter<int>("N"))
{
	consumes<CaloTowerCollection>(caloSrc_);
	produces<double>("sum");
	produces<double>("plus");
	produces<double>("minus");
	produces<double>("Wsum");
	produces<double>("Wplus");
	produces<double>("Wminus");
}

QWCaloQProducer::~QWCaloQProducer() {
	return;
}

void QWCaloQProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace reco;

	std::complex<double> HFsum(0., 0.);
	std::complex<double> HFplus(0., 0.);
	std::complex<double> HFminus(0., 0.);
	double Wsum = 0.;
	double Wplus = 0.;
	double Wminus = 0.;

	Handle<CaloTowerCollection> caloCollection;
	iEvent.getByLabel(caloSrc_, caloCollection);

	if ( caloCollection.isValid() ) {
		for ( auto j = caloCollection->begin(); j != caloCollection->end(); j++ ) {
			double eta = j->eta();
			double phi = j->phi();
			double et = j->emEt() + j->hadEt();

			std::complex<double> Q(TMath::Cos(N_*phi), TMath::Sin(N_*phi));
			if ( fabs(eta) > etaMin_ and fabs(eta) < etaMax_ ) {
				HFsum += et * Q;
				Wsum += et;
				if ( eta > 0 ) {
					HFplus += et * Q;
					Wplus += et;
				} else {
					HFminus += et * Q;
					Wminus += et;
				}
			}
		}
	}


	double pHFsum = std::arg(HFsum);
	double pHFplus = std::arg(HFplus);
	double pHFminus = std::arg(HFminus);
	iEvent.put(std::auto_ptr<double>(new double(pHFsum)), "sum");
	iEvent.put(std::auto_ptr<double>(new double(pHFplus)), "plus");
	iEvent.put(std::auto_ptr<double>(new double(pHFminus)), "minus");

	iEvent.put(std::auto_ptr<double>(new double(Wsum)), "Wsum");
	iEvent.put(std::auto_ptr<double>(new double(Wplus)), "Wplus");
	iEvent.put(std::auto_ptr<double>(new double(Wminus)), "Wminus");

}

DEFINE_FWK_MODULE(QWCaloQProducer);