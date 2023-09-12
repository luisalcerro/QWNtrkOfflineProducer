# Instructions to read over raw data

## Setup
```
- cmsrel CMSSW_13_2_2
- cd CMSSW_13_2_2/src
- mkdir OfflineProducer
- cd OfflineProducer
- git clone -b ZDC2023 https://github.com/luisalcerro/QWNtrkOfflineProducer QWNtrkOfflineProducer-ZDC2022
- cd ../
- scram b -j 8
- cmsenv
  
```
## Run over e.g random trigger pp data

```
- cd OfflineProducer/QWNtrkOfflineProducer-ZDC2022/run2021
- cmsRun recHitRAW_cfg.py runNumber=373060 runInputDir=/eos/cms/tier0/store/data/Run2023E/SpecialRandom1/RAW/v1/000/  outputTag=myoutput hlt=HLT_Random_HighRate_v1
```