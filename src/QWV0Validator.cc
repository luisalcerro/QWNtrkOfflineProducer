// -*- C++ -*-
//
// Package:    QWV0Validator
// Class:      QWV0Validator
//
/**\class QWV0Validator QWV0Validator.cc QWAna/QWCumuDiff/src/QWV0Validator.cc

Description: Creates validation histograms for RecoVertex/V0Producer

Implementation: in cmssw official release
*/
//
// Original Author:  Brian Drell
//         Created:  Wed Feb 18 17:21:04 MST 2009
// Modified by Quan Wang
//

#include <array>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
//#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
//#include "TrackingTools/PatternTools/interface/TrajectoryStateClosestToBeamLineBuilder.h"
//#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
//#include "DataFormats/V0Candidate/interface/V0Candidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

//#include "SimTracker/TrackHistory/interface/TrackClassifier.h"
//#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

//#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
//#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
//#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
//#include "Geometry/CommonDetUnit/interface/GeomDet.h"
//#include "Geometry/CommonDetUnit/interface/GluedGeomDet.h"

//#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
//#include "MagneticField/VolumeBasedEngine/interface/VolumeBasedMagneticField.h"

//#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

//#include "HepMC/GenVertex.h"
//#include "HepMC/GenParticle.h"

#include "TH3.h"

class QWV0Validator : public edm::EDAnalyzer {
    public:
        explicit QWV0Validator(const edm::ParameterSet &);
        ~QWV0Validator() override;
        enum V0Type { KSHORT, LAMBDA };
        struct V0Couple {
            reco::TrackRef one;
            reco::TrackRef two;
            explicit V0Couple(reco::TrackRef first_daughter,
                    reco::TrackRef second_daughter) {
                one = first_daughter.key() < second_daughter.key() ? first_daughter
                    : second_daughter;
                two = first_daughter.key() > second_daughter.key() ? first_daughter
                    : second_daughter;
                assert(one != two);
            }
            bool operator<(const V0Couple &rh) const {
                return one.key() < rh.one.key();
            }
            bool operator==(const V0Couple &rh) const {
                return ((one.key() == rh.one.key()) && (two.key() == rh.two.key()));
            }
        };

    private:
        void analyze(const edm::Event &, const edm::EventSetup &) override;
        void doFakeRates(const reco::VertexCompositeCandidateCollection &collection,
                const reco::RecoToSimCollection &recotosimCollection,
                V0Type t, int particle_pdgid,
                int misreconstructed_particle_pdgid);

        void doEfficiencies(
                const TrackingVertexCollection &gen_vertices, V0Type t,
                int parent_particle_id,
                int first_daughter_id,  /* give only positive charge */
                int second_daughter_id, /* give only positive charge */
                const reco::VertexCompositeCandidateCollection &collection,
                const reco::SimToRecoCollection &simtorecoCollection);

        // pt, eta, R?

        std::array<TH3D *, 2> candidateEff_num_;
        std::array<TH3D *, 2> candidateTkEff_num_;
        std::array<TH3D *, 2> candidateFake_num_;
        std::array<TH3D *, 2> candidateTkFake_num_;

        std::array<TH3D *, 2> candidateFake_denom_;
        std::array<TH3D *, 2> candidateEff_denom_;

        std::array<TH1D *, 2> nCandidates_;
        std::array<TH1D *, 2> candidateStatus_;
        std::array<TH1D *, 2> fakeCandidateMass_;
        std::array<TH1D *, 2> candidateFakeDauRadDist_;
        std::array<TH1D *, 2> candidateMassAll;
        std::array<TH1D *, 2> goodCandidateMass;

        edm::EDGetTokenT<reco::RecoToSimCollection> recoRecoToSimCollectionToken_;
        edm::EDGetTokenT<reco::SimToRecoCollection> recoSimToRecoCollectionToken_;
        edm::EDGetTokenT<TrackingVertexCollection> trackingVertexCollection_Token_;
        edm::EDGetTokenT<reco::VertexCompositeCandidateCollection>
            recoVertexCompositeCandidateCollection_k0s_Token_,
            recoVertexCompositeCandidateCollection_lambda_Token_;
};

typedef std::vector<TrackingVertex> TrackingVertexCollection;
typedef edm::Ref<TrackingVertexCollection> TrackingVertexRef;
typedef edm::RefVector<edm::HepMCProduct, HepMC::GenVertex> GenVertexRefVector;
typedef edm::RefVector<edm::HepMCProduct, HepMC::GenParticle>
GenParticleRefVector;

QWV0Validator::QWV0Validator(const edm::ParameterSet& iConfig) :
    recoRecoToSimCollectionToken_(consumes<reco::RecoToSimCollection>(
                iConfig.getUntrackedParameter<edm::InputTag>("trackAssociatorMap"))),
    recoSimToRecoCollectionToken_(consumes<reco::SimToRecoCollection>(
                iConfig.getUntrackedParameter<edm::InputTag>("trackAssociatorMap"))),
    trackingVertexCollection_Token_(consumes<TrackingVertexCollection>(
                iConfig.getUntrackedParameter<edm::InputTag>(
                    "trackingVertexCollection"))),
    recoVertexCompositeCandidateCollection_k0s_Token_(
            consumes<reco::VertexCompositeCandidateCollection>(
                iConfig.getUntrackedParameter<edm::InputTag>(
                    "kShortCollection"))),
    recoVertexCompositeCandidateCollection_lambda_Token_(
            consumes<reco::VertexCompositeCandidateCollection>(
                iConfig.getUntrackedParameter<edm::InputTag>(
                    "lambdaCollection")))
{
    edm::Service<TFileService> fs;
    double minKsMass = 0.43;
    double maxKsMass = 0.565;
    double minLamMass = 1.08;
    double maxLamMass = 1.16;
    int ksMassNbins = 270;
    int lamMassNbins = 160;

    std::vector<double> vKsMass, vLmMass;
    vKsMass.reserve(ksMassNbins+1);
    vLmMass.reserve(lamMassNbins+1);
    {
        double dmass = (maxKsMass - minKsMass)/ksMassNbins;
        for ( int i = 0; i < ksMassNbins; i++ ) {
            vKsMass.push_back(minKsMass + dmass*i);
        }
        vKsMass.push_back(maxKsMass);

        dmass = (maxLamMass - minLamMass)/lamMassNbins;
        for ( int i = 0; i < lamMassNbins; i++ ) {
            vLmMass.push_back(minLamMass + dmass*i);
        }
        vLmMass.push_back(maxLamMass);
    }

    std::vector<double> etaBins{-2.5, -2.4, -2.3, -2.2, -2.1, -2.0,
        -1.9, -1.8, -1.7, -1.6, -1.5, -1.4, -1.3, -1.2, -1.1, -1.0,
        -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0,
        0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0,
        1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0,
        2.1, 2.2, 2.3, 2.4, 2.5};

    std::vector<double> pTbins{0.2, 0.4, 0.6, 0.8, 1.0, 1.4, 1.8, 2.2, 2.8, 3.6, 4.6,6.0, 7.0, 8.5};

    double maxR = 40.;
    int NR = 80;
    std::vector<double> Rbins;
    Rbins.reserve(NR);
    {
        double dR = maxR / NR;
        for ( int i = 0; i < NR; i++ ) {
            Rbins.push_back( dR*i );
        }
        Rbins.push_back(maxR);
    }

    // pT, eta, R
    auto fKs = fs->mkdir("Ks");
    candidateEff_num_[QWV0Validator::KSHORT] = fKs.make<TH3D>(
            "K0sEff_num", "K0sEff_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );
    candidateTkEff_num_[QWV0Validator::KSHORT] = fKs.make<TH3D>(
            "K0sTkEff_num", "K0sTkEff_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );

    candidateEff_denom_[QWV0Validator::KSHORT] = fKs.make<TH3D>(
            "K0sEff_denom", "K0sEff_denom:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );

    candidateFake_num_[QWV0Validator::KSHORT] = fKs.make<TH3D>(
            "K0sFake_num", "K0sFake_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );
    candidateTkFake_num_[QWV0Validator::KSHORT] = fKs.make<TH3D>(
            "K0sTkFake_num", "K0sTkFake_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );

    candidateFake_denom_[QWV0Validator::KSHORT] = fKs.make<TH3D>(
            "K0sFake_denom", "K0sFake_denom:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );

    nCandidates_[QWV0Validator::KSHORT] = fKs.make<TH1D>(
            "nK0s", "Number of K^{0}_{S} found per event", 60, 0., 60.);
    fakeCandidateMass_[QWV0Validator::KSHORT] = fKs.make<TH1D>(
            "ksMassFake", "Mass of fake K0S", ksMassNbins, minKsMass, maxKsMass);
    goodCandidateMass[QWV0Validator::KSHORT] = fKs.make<TH1D>(
            "ksMassGood", "Mass of good reco K0S", ksMassNbins, minKsMass, maxKsMass);
    candidateMassAll[QWV0Validator::KSHORT] =
        fKs.make<TH1D>("ksMassAll", "Invariant mass of all K0S", ksMassNbins,
                minKsMass, maxKsMass);
    candidateFakeDauRadDist_[QWV0Validator::KSHORT] = fKs.make<TH1D>(
            "radDistFakeKs", "Production radius of daughter particle of Ks fake", 100,
            0., 15.);
    candidateStatus_[QWV0Validator::KSHORT] =
        fKs.make<TH1D>("ksCandStatus", "Fake type by cand status", 10, 0., 10.);

    // Lambda Plots follow

    auto fLm = fs->mkdir("Lm");
    candidateEff_num_[QWV0Validator::LAMBDA] = fLm.make<TH3D>(
            "LamEff_num", "LmEff_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );
    candidateTkEff_num_[QWV0Validator::LAMBDA] = fLm.make<TH3D>(
            "LamTkEff_num", "LmTkEff_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );
    candidateEff_denom_[QWV0Validator::LAMBDA] = fLm.make<TH3D>(
            "LamEff_denom", "LmEff_denom:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );

    candidateFake_num_[QWV0Validator::LAMBDA] = fLm.make<TH3D>(
            "LamFake_num", "LmFake_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );
    candidateTkFake_num_[QWV0Validator::LAMBDA] = fLm.make<TH3D>(
            "LamTkFake_num", "LmTkFake_num:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );
    candidateFake_denom_[QWV0Validator::LAMBDA] = fLm.make<TH3D>(
            "LamFake_denom", "LmFake_denom:p_{T}:#eta:R", pTbins.size()-1, pTbins.data(),
            etaBins.size()-1, etaBins.data(), Rbins.size()-1, Rbins.data() );


    nCandidates_[QWV0Validator::LAMBDA] = fLm.make<TH1D>(
            "nLam", "Number of #Lambda^{0} found per event", 60, 0., 60.);
    fakeCandidateMass_[QWV0Validator::LAMBDA] =
        fLm.make<TH1D>("lamMassFake", "Mass of fake Lambda", lamMassNbins,
                minLamMass, maxLamMass);
    goodCandidateMass[QWV0Validator::LAMBDA] =
        fLm.make<TH1D>("lamMassGood", "Mass of good Lambda", lamMassNbins,
                minLamMass, maxLamMass);

    candidateMassAll[QWV0Validator::LAMBDA] =
        fLm.make<TH1D>("lamMassAll", "Invariant mass of all #Lambda^{0}",
                lamMassNbins, minLamMass, maxLamMass);
    candidateFakeDauRadDist_[QWV0Validator::LAMBDA] = fLm.make<TH1D>(
            "radDistFakeLam", "Production radius of daughter particle of Lam fake",
            100, 0., 15.);

    candidateStatus_[QWV0Validator::LAMBDA] =
        fLm.make<TH1D>("lamCandStatus", "Fake type by cand status", 10, 0., 10.);
}

QWV0Validator::~QWV0Validator() {}

void QWV0Validator::doFakeRates(
        const reco::VertexCompositeCandidateCollection& collection,
        const reco::RecoToSimCollection& recotosimCollection, V0Type v0_type,
        int particle_pdgid, int misreconstructed_particle_pdgid) {

    using namespace edm;

    int numCandidateFound = 0;
    int realCandidateFound = 0;
    double mass = 0.;
    float CandidatepT = 0.;
    float CandidateEta = 0.;
    float CandidateR = 0.;
    int CandidateStatus = 0;
    const unsigned int NUM_DAUGHTERS = 2;
    if (!collection.empty()) {
        for (reco::VertexCompositeCandidateCollection::const_iterator iCandidate = collection.begin();
                iCandidate != collection.end(); iCandidate++) {

            // Fill values to be histogrammed
            mass = iCandidate->mass();
            CandidatepT = (sqrt(iCandidate->momentum().perp2()));
            //CandidateEta = iCandidate->momentum().eta();
            CandidateEta = iCandidate->rapidity();
            CandidateR = (sqrt(iCandidate->vertex().perp2()));
            candidateMassAll[v0_type]->Fill(mass);
            CandidateStatus = 0;

            std::array<reco::TrackRef, NUM_DAUGHTERS> theDaughterTracks = {
                {(*(dynamic_cast<const reco::RecoChargedCandidate*>(
                                iCandidate->daughter(0)))).track(),
                (*(dynamic_cast<const reco::RecoChargedCandidate*>(
                                                                   iCandidate->daughter(1)))).track()}};

            TrackingParticleRef tpref;
            TrackingParticleRef firstDauTP;
            TrackingVertexRef candidateVtx;

            std::array<double, NUM_DAUGHTERS> radDist;
            // Loop through candidate's daugher tracks
            for (View<reco::Track>::size_type i = 0; i < theDaughterTracks.size();
                    ++i) {
                radDist = {{-1., -1.}};
                // Found track from theDaughterTracks
                RefToBase<reco::Track> track(theDaughterTracks.at(i));

                if (recotosimCollection.find(track) != recotosimCollection.end()) {
                    const std::vector<std::pair<TrackingParticleRef, double> >& tp =
                        recotosimCollection[track];
                    if (!tp.empty()) {
                        tpref = tp.begin()->first;

                        TrackingVertexRef parentVertex = tpref->parentVertex();
                        if (parentVertex.isNonnull()) {
                            radDist[i] = parentVertex->position().R();
                            if (candidateVtx.isNonnull()) {
                                if (candidateVtx->position() == parentVertex->position()) {
                                    if (parentVertex->nDaughterTracks() == 2) {
                                        if (parentVertex->nSourceTracks() == 0) {
                                            // No source tracks found for candidate's
                                            // vertex: it shouldn't happen, but does for
                                            // evtGen events
                                            CandidateStatus = 6;
                                        }

                                        for (TrackingVertex::tp_iterator iTP =
                                                parentVertex->sourceTracks_begin();
                                                iTP != parentVertex->sourceTracks_end(); iTP++) {
                                            if (abs((*iTP)->pdgId()) == particle_pdgid) {
                                                CandidateStatus = 1;
                                                realCandidateFound++;
                                                numCandidateFound += 1.;
                                                goodCandidateMass[v0_type]->Fill(mass);
                                            } else {
                                                CandidateStatus = 2;
                                                if (abs((*iTP)->pdgId()) ==
                                                        misreconstructed_particle_pdgid) {
                                                    CandidateStatus = 7;
                                                }
                                            }
                                        }
                                    } else {
                                        // Found a bad match because the mother has too
                                        // many daughters
                                        CandidateStatus = 3;
                                    }
                                } else {
                                    // Found a bad match because the parent vertices
                                    // from the two tracks are different
                                    CandidateStatus = 4;
                                }
                            } else {
                                // if candidateVtx is null, fill it with parentVertex
                                // to compare to the parentVertex from the second
                                // track
                                candidateVtx = parentVertex;
                                firstDauTP = tpref;
                            }
                        }  // parent vertex is null
                    }    // check on associated tp size zero
                } else {
                    CandidateStatus = 5;
                }
            }  // Loop on candidate's daughter tracks

            // fill the fake rate histograms
            if (CandidateStatus > 1) {
                candidateFake_num_[v0_type]->Fill(CandidatepT, CandidateEta, CandidateR);
                candidateStatus_[v0_type]->Fill((float)CandidateStatus);
                fakeCandidateMass_[v0_type]->Fill(mass);
                for (auto distance : radDist) {
                    if (distance > 0) candidateFakeDauRadDist_[v0_type]->Fill(distance);
                }
            }
            if (CandidateStatus == 5) {
                candidateTkFake_num_[v0_type]->Fill(CandidatepT, CandidateEta, CandidateR);
            }
            candidateFake_denom_[v0_type]->Fill(CandidatepT, CandidateEta, CandidateR);
        }  // Loop on candidates
    }    // check on presence of candidate's collection in the event
    nCandidates_[v0_type]->Fill((float)numCandidateFound);
}

void QWV0Validator::doEfficiencies(
        const TrackingVertexCollection& gen_vertices, V0Type v0_type,
        int parent_particle_id,
        int first_daughter_id,  /* give only positive charge */
        int second_daughter_id, /* give only positive charge */
        const reco::VertexCompositeCandidateCollection& collection,
        const reco::SimToRecoCollection& simtorecoCollection) {
    /* We store the TrackRef of the tracks that have been used to
     * produce the V0 under consideration here. This is used later to
     * check if a specific V0 has been really reconstructed or not. The
     * ordering is based on the key_index of the reference, since it
     * indeed does not matter that much. */

    std::set<V0Couple> reconstructed_V0_couples;
    if (!collection.empty()) {
        for (reco::VertexCompositeCandidateCollection::const_iterator iCandidate =
                collection.begin();
                iCandidate != collection.end(); iCandidate++) {
            reconstructed_V0_couples.insert(
                    V0Couple((dynamic_cast<const reco::RecoChargedCandidate*>(
                                iCandidate->daughter(0)))->track(),
                        (dynamic_cast<const reco::RecoChargedCandidate*>(
                                                                         iCandidate->daughter(1)))->track()));
        }
    }

    /* PSEUDO CODE
       for v in gen_vertices
       if v.eventId().BX() !=0 continue
       if v.nDaughterTracks != 2 continue
       for source in v.sourceTracks_begin
       if source is parent_particle_id
       for daughter in v.daughterTracks_begin
       if daughter in region_and_kine_cuts
       decay_found
       */
    unsigned int candidateEff[2] = {0, 0};
    for (auto const& gen_vertex : gen_vertices) {
        if (gen_vertex.eventId().bunchCrossing() != 0)
            continue;  // Consider only in-time events
        if (gen_vertex.nDaughterTracks() != 2) continue;  // Keep only V0 vertices
        for (TrackingVertex::tp_iterator source = gen_vertex.sourceTracks_begin();
                source != gen_vertex.sourceTracks_end(); ++source) {
            if (std::abs((*source)->pdgId()) == parent_particle_id) {
                if ((std::abs((gen_vertex.daughterTracks().at(0))->pdgId()) ==
                            first_daughter_id &&
                            std::abs((gen_vertex.daughterTracks().at(1))->pdgId()) ==
                            second_daughter_id) ||
                        (std::abs((gen_vertex.daughterTracks().at(0))->pdgId()) ==
                         second_daughter_id &&
                         std::abs((gen_vertex.daughterTracks().at(1))->pdgId()) ==
                         first_daughter_id)) {
                    if ((std::abs((gen_vertex.daughterTracks().at(0))->momentum().eta()) <
                                2.4 &&
                                gen_vertex.daughterTracks().at(0)->pt() > 0.9) &&
                            (std::abs((gen_vertex.daughterTracks().at(1))->momentum().eta()) <
                             2.4 &&
                             gen_vertex.daughterTracks().at(1)->pt() > 0.9)) {
                        // found desired generated Candidate
                        float candidateGenpT = sqrt((*source)->momentum().perp2());
                        float candidateGenEta = (*source)->momentum().eta();
                        float candidateGenR = sqrt((*source)->vertex().perp2());
                        candidateEff_denom_[v0_type]->Fill(candidateGenpT, candidateGenEta, candidateGenR);

                        std::array<reco::TrackRef, 2> reco_daughter;

                        for (unsigned int daughter = 0; daughter < 2; ++daughter) {
                            if (simtorecoCollection.find(
                                        gen_vertex.daughterTracks()[daughter]) !=
                                    simtorecoCollection.end()) {
                                if (!simtorecoCollection[gen_vertex.daughterTracks()[daughter]].empty()) {
                                    candidateEff[daughter] = 1;  // Found a daughter track
                                    reco_daughter[daughter] =
                                        simtorecoCollection[gen_vertex.daughterTracks()[daughter]]
                                        .begin()
                                        ->first.castTo<reco::TrackRef>();
                                }
                            } else {
                                candidateEff[daughter] = 2;  // First daughter not found
                            }
                        }
                        if ((candidateEff[0] == 1 && candidateEff[1] == 1) &&
                                (reco_daughter[0].key() != reco_daughter[1].key()) &&
                                (reconstructed_V0_couples.find(
                                                               V0Couple(reco_daughter[0], reco_daughter[1])) !=
                                 reconstructed_V0_couples.end())) {
                            candidateEff_num_[v0_type]->Fill(candidateGenpT, candidateGenEta, candidateGenR);
                        }
                    }  // Check that daughters are inside the desired kinematic region
                }    // Check decay products of the current generatex vertex
            }      // Check pdgId of the source of the current generated vertex
        }        // Loop over all sources of the current generated vertex
    }          // Loop over all generated vertices
}

void QWV0Validator::analyze(const edm::Event& iEvent,
        const edm::EventSetup& iSetup) {
    using std::cout;
    using std::endl;
    using namespace edm;
    using namespace std;

    // Make matching collections
    Handle<reco::RecoToSimCollection> recotosimCollectionH;
    iEvent.getByToken(recoRecoToSimCollectionToken_, recotosimCollectionH);

    Handle<reco::SimToRecoCollection> simtorecoCollectionH;
    iEvent.getByToken(recoSimToRecoCollectionToken_, simtorecoCollectionH);

    // Get Monte Carlo information
    edm::Handle<TrackingVertexCollection> TVCollectionH;
    iEvent.getByToken(trackingVertexCollection_Token_, TVCollectionH);

    // get the V0s;
    edm::Handle<reco::VertexCompositeCandidateCollection> k0sCollection;
    edm::Handle<reco::VertexCompositeCandidateCollection> lambdaCollection;
    iEvent.getByToken(recoVertexCompositeCandidateCollection_k0s_Token_,
            k0sCollection);
    iEvent.getByToken(recoVertexCompositeCandidateCollection_lambda_Token_,
            lambdaCollection);

    // Do fake rate and efficiency calculation

    // Get gen vertex collection out of the event, as done in the Vertex
    // validation package!!!
    if (k0sCollection.isValid()) {
        doFakeRates(*k0sCollection.product(), *recotosimCollectionH.product(),
                V0Type::KSHORT, 310, 3122);
        doEfficiencies(*TVCollectionH.product(), V0Type::KSHORT, 310, 211, 211,
                *k0sCollection.product(), *simtorecoCollectionH.product());
    }
    if (lambdaCollection.isValid()) {
        doFakeRates(*lambdaCollection.product(), *recotosimCollectionH.product(),
                V0Type::LAMBDA, 3122, 310);
        doEfficiencies(*TVCollectionH.product(), V0Type::LAMBDA, 3122, 211, 2212,
                *lambdaCollection.product(),
                *simtorecoCollectionH.product());
    }
}

// define this as a plug-in
DEFINE_FWK_MODULE(QWV0Validator);
