import FWCore.ParameterSet.Config as cms

process = cms.Process("ZDC")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')


process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, '75X_dataRun2_v13', '')
#
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_Prompt_v16', '')

process.options = cms.untracked.PSet(
    Rethrow = cms.untracked.vstring('ProductNotFound')
)

process.source = cms.Source("PoolSource",
#        fileNames = cms.untracked.vstring("file:/afs/cern.ch/user/q/qwang/work/cleanroomRun2/Ana/CMSSW_7_5_8_patch2/src/QWAna/QWCumuV3/test/HIMinBias_28.root")
	fileNames = cms.untracked.vstring("file:pickevents.root")
#	fileNames = cms.untracked.vstring("file:/afs/cern.ch/user/q/qwang/work/cleanroomRun2/Ana/data/pPb_8_FEVT.root")
#	fileNames = cms.untracked.vstring("file:/eos/cms/store/hidata/PARun2016C/PAHighMultiplicity0/AOD/PromptReco-v1/000/286/201/00000/0695AC66-FFB9-E611-A015-02163E01478E.root")
#	fileNames = cms.untracked.vstring("root://cms-xrd-global.cern.ch//store/hidata/PARun2016C/PAMinimumBias2/AOD/PromptReco-v1/000/286/178/00000/00C9804B-BEB9-E611-9F3E-FA163E2E1963.root'")
#	fileNames = cms.untracked.vstring("root://cms-xrd-global.cern.ch//store/hidata/PARun2016C/PAMinimumBias1/AOD/PromptReco-v1/000/286/178/00000/0ACB2C21-BDB9-E611-9F95-02163E01231F.root",
#		"root://cms-xrd-global.cern.ch//store/hidata/PARun2016C/PAMinimumBias1/AOD/PromptReco-v1/000/286/178/00000/08AEE0E9-BEB9-E611-9813-02163E01354E.root",
#		"root://cms-xrd-global.cern.ch//store/hidata/PARun2016C/PAMinimumBias1/AOD/PromptReco-v1/000/286/178/00000/08E45934-BBB9-E611-8B47-02163E0146FA.root",
#		"root://cms-xrd-global.cern.ch//store/hidata/PARun2016C/PAMinimumBias1/AOD/PromptReco-v1/000/286/178/00000/0A3A6BEF-BEB9-E611-A6DF-02163E014626.root"
#		)
)

import HLTrigger.HLTfilters.hltHighLevel_cfi

#process.hltMB = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
#process.hltMB.HLTPaths = [
#	"HLT_HIL1MinimumBiasHF2AND_*",
#	"HLT_HIL1MinimumBiasHF1AND_*",
#]
#process.hltMB.andOr = cms.bool(True)
#process.hltMB.throw = cms.bool(False)

#process.TFileService = cms.Service("TFileService",
#    fileName = cms.string('tracking.root')
#)
process.hltMB = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltMB.HLTPaths = [
		"HLT_PAL1MinimumBiasHF_OR_SinglePixelTrack_part*",
		]
process.hltMB.andOr = cms.bool(True)
process.hltMB.throw = cms.bool(False)



process.load('QWAna.QWNtrkOfflineProducer.QWZDC_cfi')


process.path = cms.Path(process.hltMB * process.QWZDC)

process.RECO = cms.OutputModule("PoolOutputModule",
                outputCommands = cms.untracked.vstring('keep *_QWZDC_*_*'),
                SelectEvents = cms.untracked.PSet(
                        SelectEvents = cms.vstring('path')
                        ),
                fileName = cms.untracked.string('zdc.root')
                )


process.out = cms.EndPath(process.RECO)

process.schedule = cms.Schedule(
	process.path,
	process.out
)
