#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
//#include "MagneticField/Engine/interface/MagneticField.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"

#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidateFwd.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"

#include "TFile.h"
#include "TH2.h"
#include "TMath.h"
#include <Math/SMatrix.h>
#include <Math/SVector.h>

typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;
typedef ROOT::Math::SVector<double, 3> SVector3;

using namespace std;
class QWV0VectProducer : public edm::EDProducer {
public:
	explicit QWV0VectProducer(const edm::ParameterSet&);
	~QWV0VectProducer();

private:
	virtual void produce(edm::Event&, const edm::EventSetup&) override;
	///

	edm::InputTag	vertexSrc_;
	edm::InputTag	trackSrc_;

	edm::InputTag	V0Src_;

	typedef struct {
		double	pTmin_;
		double	pTmax_;
		double	Etamin_;
		double	Etamax_;
		double	Massmin_;
		double	Massmax_;

		double	Chi2min_;
		double	Chi2max_;

		double	Ndfmin_;
		double	Ndfmax_;

		double	Chi2oNdfmin_;
		double	Chi2oNdfmax_;

		double	Lxymin_;
		double	Lxymax_;

		double	Lxyzmin_;
		double	Lxyzmax_;

		double	ThetaXYmin_;
		double	ThetaXYmax_;

		double	ThetaXYZmin_;
		double	ThetaXYZmax_;

		double	DecaySigXYmin_;
		double	DecaySigXYmax_;

		double	DecaySigXYZmin_;
		double	DecaySigXYZmax_;

		double	DCAmin_;
		double	DCAmax_;

		double	VtxProbmin_;
		double	VtxProbmax_;
	} V0_cut;
	std::vector<QWV0VectProducer::V0_cut> cuts_;
};

QWV0VectProducer::QWV0VectProducer(const edm::ParameterSet& pset) :
	vertexSrc_(pset.getUntrackedParameter<edm::InputTag>("vertexSrc")),
	trackSrc_(pset.getUntrackedParameter<edm::InputTag>("trackSrc")),
	V0Src_(pset.getUntrackedParameter<edm::InputTag>("V0Src"))
{

	auto pcuts = pset.getUntrackedParameter< std::vector< edm::ParameterSet > >("cuts");
	for ( auto pcut : pcuts ) {
		QWV0VectProducer::V0_cut cut;

		cut.pTmin_ = pset.getUntrackedParameter<double>("ptMin", 0.);
		cut.pTmax_ = pset.getUntrackedParameter<double>("ptMax", 100.);
		cut.Etamin_ = pset.getUntrackedParameter<double>("Etamin", -2.4);
		cut.Etamax_ = pset.getUntrackedParameter<double>("Etamax", 2.4);
		cut.Massmin_ = pset.getUntrackedParameter<double>("Massmin", 0.);
		cut.Massmax_ = pset.getUntrackedParameter<double>("Massmax", 1000);

		cut.Chi2min_ = pset.getUntrackedParameter<double>("Chi2Min", -999999.);
		cut.Chi2max_ = pset.getUntrackedParameter<double>("Chi2Max",  999999.);

		cut.Ndfmin_ = pset.getUntrackedParameter<double>("NdfMin", -999999.);
		cut.Ndfmax_ = pset.getUntrackedParameter<double>("NdfMax",  999999.);

		cut.Chi2oNdfmin_ = pset.getUntrackedParameter<double>("Chi2oNdfMin", -999999.);
		cut.Chi2oNdfmax_ = pset.getUntrackedParameter<double>("Chi2oNdfMax",  999999.);

		cut.Lxymin_ = pset.getUntrackedParameter<double>("LxyMin", -999999.);
		cut.Lxymax_ = pset.getUntrackedParameter<double>("LxyMax",  999999.);

		cut.Lxyzmin_ = pset.getUntrackedParameter<double>("LxyzMin", -999999.);
		cut.Lxyzmax_ = pset.getUntrackedParameter<double>("LxyzMax",  999999.);

		cut.ThetaXYmin_ = pset.getUntrackedParameter<double>("ThetaXYMin", -999999.);
		cut.ThetaXYmax_ = pset.getUntrackedParameter<double>("ThetaXYMax",  999999.);

		cut.ThetaXYZmin_ = pset.getUntrackedParameter<double>("ThetaXYZMin", -999999.);
		cut.ThetaXYZmax_ = pset.getUntrackedParameter<double>("ThetaXYZMax",  999999.);

		cut.DecaySigXYmin_ = pset.getUntrackedParameter<double>("DecayXYMin", -999999.);
		cut.DecaySigXYmax_ = pset.getUntrackedParameter<double>("DecayXYMax",  999999.);

		cut.DecaySigXYZmin_ = pset.getUntrackedParameter<double>("DecayXYZMin", -999999.);
		cut.DecaySigXYZmax_ = pset.getUntrackedParameter<double>("DecayXYZMax",  999999.);

		cut.DCAmin_ = pset.getUntrackedParameter<double>("DCAMin", -999999.);
		cut.DCAmax_ = pset.getUntrackedParameter<double>("DCAMax",  999999.);


		cut.VtxProbmin_ = pset.getUntrackedParameter<double>("VtxProbmin", -1.);
		cut.VtxProbmax_ = pset.getUntrackedParameter<double>("VtxProbmax", 10.);

		cuts_.push_back(cut);
	}


	consumes<reco::TrackCollection>(trackSrc_);
	consumes<reco::VertexCollection>(vertexSrc_);
	consumes<reco::VertexCompositeCandidateCollection>(V0Src_);



	produces<std::vector<double> >("phi");
	produces<std::vector<double> >("eta");
	produces<std::vector<double> >("pt");
	produces<std::vector<double> >("mass");
	produces<std::vector<double> >("weight");

	produces<std::vector<double> >("vtxChi2");
	produces<std::vector<double> >("vtxNdf");
	produces<std::vector<double> >("vtxChi2oNdf");

	produces<std::vector<double> >("Lxy");
	produces<std::vector<double> >("Lxyz");

	produces<std::vector<double> >("cosThetaXY");
	produces<std::vector<double> >("cosThetaXYZ");

	produces<std::vector<double> >("vtxDecaySigXY");
	produces<std::vector<double> >("vtxDecaySigXYZ");

	produces<std::vector<double> >("DCA");

	produces<std::vector<double> >("Refs");
}

QWV0VectProducer::~QWV0VectProducer()
{
	return;
}

void QWV0VectProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace reco;

	std::auto_ptr<std::vector<double> > pphi( new std::vector<double> );
	std::auto_ptr<std::vector<double> > peta( new std::vector<double> );
	std::auto_ptr<std::vector<double> > ppT( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pmass( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pweight( new std::vector<double> );

	std::auto_ptr<std::vector<double> > pvtxChi2( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pvtxNdf( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pvtxChi2oNdf( new std::vector<double> );

	std::auto_ptr<std::vector<double> > pLxy( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pLxyz( new std::vector<double> );

	std::auto_ptr<std::vector<double> > pcosThetaXY( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pcosThetaXYZ( new std::vector<double> );

	std::auto_ptr<std::vector<double> > pvtxDecaySigXY( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pvtxDecaySigXYZ( new std::vector<double> );

	std::auto_ptr<std::vector<double> > pDCA( new std::vector<double> );
	std::auto_ptr<std::vector<double> > pRefs( new std::vector<double> );

	edm::ESHandle<TransientTrackBuilder> theB;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);

//	edm::Handle<reco::TrackCollection> theTrackHandle;
//	iEvent.getByLabel(trackSrc_, theTrackHandle);
//	if (!theTrackHandle->size()) return;
//	const reco::TrackCollection* theTrackCollection = theTrackHandle.product();


	edm::Handle< reco::VertexCollection > pvHandle;
	iEvent.getByLabel(vertexSrc_, pvHandle);
	reco::Vertex const * pv = nullptr;  
	size_t nPV = 0;
	int const pvNDOF_ = 4; // hardcoded
	if (pvHandle.isValid()) {
//		pv = &pvHandle->front();
//		//--- pv fake (the pv collection should have size==1 and the pv==beam spot)
//		if (   pv->isFake() || pv->tracksSize()==0
//			// definition of goodOfflinePrimaryVertex
//			|| pv->ndof() < pvNDOF_ || pv->z() > 24.)  pv = nullptr;


		for (auto& v : *pvHandle) {
			//--- pv fake (the pv collection should have size==1 and the pv==beam spot)
			if (  v.isFake() || v.tracksSize()==0 ) continue;
			// definition of goodOfflinePrimaryVertex
			if (  v.ndof() < pvNDOF_ || v.z() > 24.) continue;
			++nPV;
			if ( pv == nullptr ) pv = &v;
		}
	}
	if ( pv == nullptr ) return;

	Handle<VertexCompositeCandidateCollection> V0s;
	iEvent.getByLabel(V0Src_, V0s);

	for ( auto & v0 : (*V0s) ) {
		float mass     = v0.mass();
		float pt       = v0.pt();
		float eta      = v0.eta();
		float phi      = v0.phi();
		GlobalPoint displacementFromPV ( (pv->x() - v0.vx()), (pv->y() - v0.vy()), (pv->z() - v0.vz()) ) ;
//		GlobalPoint momentum = GlobalPoint(v0.px(), v0.py(), v0.pz());
		float lxy      = ( displacementFromPV.perp() );
		float lxyz     = ( displacementFromPV.mag() );

		double Chi2 = v0.vertexChi2();
		double Ndf  = v0.vertexNdof();
		double chi2oNDF = v0.vertexNormalizedChi2();
		double VtxProb = TMath::Prob(Chi2, Ndf);

		double	cosThetaXY  = -(displacementFromPV.x()*v0.px() + displacementFromPV.y()*v0.py()) / ( displacementFromPV.transverse()*sqrt(v0.px()*v0.px() + v0.py()*v0.py()) );
		double	cosThetaXYZ = -(displacementFromPV.x()*v0.px() + displacementFromPV.y()*v0.py() + displacementFromPV.z()*v0.pz()) / ( displacementFromPV.mag()*sqrt(v0.px()*v0.px() + v0.py()*v0.py() + v0.pz()*v0.pz()) );

		SMatrixSym3D totalCov = pv->covariance() + v0.vertexCovariance();
		SVector3 distVecXY(displacementFromPV.x(), displacementFromPV.y(), 0.);
		double distMagXY = ROOT::Math::Mag(distVecXY);
		double sigmaDistMagXY = sqrt(ROOT::Math::Similarity(totalCov, distVecXY)) / distMagXY;
		double vtxDecaySigXY = distMagXY/sigmaDistMagXY;

		SVector3 distVecXYZ(displacementFromPV.x(), displacementFromPV.y(), displacementFromPV.z());
		double distMagXYZ = ROOT::Math::Mag(distVecXYZ);
		double sigmaDistMagXYZ = sqrt(ROOT::Math::Similarity(totalCov, distVecXYZ)) / distMagXYZ;
		double vtxDecaySigXYZ = distMagXYZ/sigmaDistMagXYZ;
		double dca = -999.;
		if ( v0.numberOfDaughters() == 2 ) {
			auto bT0 = v0.daughter(0)->bestTrack();
			auto bT1 = v0.daughter(1)->bestTrack();

			if ( bT0 and bT1 ) {
				auto tt0 = theB->build(bT0).initialFreeState();
				auto tt1 = theB->build(bT1).initialFreeState();

				ClosestApproachInRPhi cApp;
				cApp.calculate(tt0, tt1);
				if (cApp.status()) {
					dca = std::abs(cApp.distance());
				}
			}
		}


		for ( auto const cut : cuts_ ) {
			if ( pt > cut.pTmax_ or pt < cut.pTmin_ ) continue;
			if ( eta > cut.Etamax_ or eta < cut.Etamin_ ) continue;
			if ( mass > cut.Massmax_ or mass < cut.Massmin_ ) continue;

			if ( Chi2 > cut.Chi2max_ or Chi2 < cut.Chi2min_ ) continue;
			if ( Ndf  > cut.Ndfmax_  or Ndf  < cut.Ndfmin_  ) continue;

			if ( chi2oNDF > cut.Chi2oNdfmax_ or chi2oNDF < cut.Chi2oNdfmin_ ) continue;

			if ( lxy > cut.Lxymax_  or lxy < cut.Lxymin_ ) continue;
			if ( lxyz> cut.Lxyzmax_ or lxyz< cut.Lxyzmin_) continue;

			if ( cosThetaXY > cut.ThetaXYmax_  or cosThetaXY < cut.ThetaXYmin_ ) continue;
			if ( cosThetaXYZ> cut.ThetaXYZmax_ or cosThetaXYZ< cut.ThetaXYZmin_) continue;

			if ( vtxDecaySigXY > cut.DecaySigXYmax_  or vtxDecaySigXY < cut.DecaySigXYmin_ ) continue;
			if ( vtxDecaySigXYZ> cut.DecaySigXYZmax_ or vtxDecaySigXYZ< cut.DecaySigXYZmin_ ) continue;

			if ( dca > cut.DCAmax_ or dca < cut.DCAmin_ ) continue;

			if ( VtxProb > cut.VtxProbmax_ or VtxProb < cut.VtxProbmin_ ) continue;

			pphi		->push_back( phi	);
			peta		->push_back( eta	);
			ppT		->push_back( pt		);
			pmass		->push_back( mass	);
			pweight		->push_back( 1.0	);

			pvtxChi2	->push_back( Chi2	);
			pvtxNdf		->push_back( Ndf	);
			pvtxChi2oNdf	->push_back( chi2oNDF		);

			pLxy		->push_back( lxy	);
			pLxyz		->push_back( lxyz	);


			pcosThetaXY	->push_back( cosThetaXY		);
			pcosThetaXYZ	->push_back( cosThetaXYZ	);


			pvtxDecaySigXY	->push_back( vtxDecaySigXY	);
			pvtxDecaySigXYZ	->push_back( vtxDecaySigXYZ	);

			pDCA	->push_back( dca	);

			if ( v0.numberOfDaughters() == 2 ) {
				for ( unsigned int i = 0; i < 2; i++ ) {
					auto dau = dynamic_cast<const RecoChargedCandidate*>(v0.daughter(i));
					double idx = -1.;
					if ( dau ) idx = dau->track().index();
					pRefs->push_back( idx );
				}
			}
		}
	}


	iEvent.put(pphi, std::string("phi"));
	iEvent.put(peta, std::string("eta"));
	iEvent.put(ppT, std::string("pt"));
	iEvent.put(pmass, std::string("mass"));
	iEvent.put(pweight, std::string("weight"));

	iEvent.put(pvtxChi2, std::string("vtxChi2"));
	iEvent.put(pvtxNdf, std::string("vtxNdf"));
	iEvent.put(pvtxChi2oNdf, std::string("vtxChi2oNdf"));

	iEvent.put(pLxy, std::string("Lxy"));
	iEvent.put(pLxyz, std::string("Lxyz"));

	iEvent.put(pcosThetaXY, std::string("cosThetaXY"));
	iEvent.put(pcosThetaXYZ, std::string("cosThetaXYZ"));

	iEvent.put(pvtxDecaySigXY, std::string("vtxDecaySigXY"));
	iEvent.put(pvtxDecaySigXYZ, std::string("vtxDecaySigXYZ"));

	iEvent.put(pDCA, std::string("DCA"));
	iEvent.put(pRefs, std::string("Refs"));
}


DEFINE_FWK_MODULE(QWV0VectProducer);
