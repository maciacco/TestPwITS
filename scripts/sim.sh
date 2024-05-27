#! /usr/bin/bash -xe

NEVENTS=1000
BOXGEN="-g boxgen --configKeyValues BoxGun.pdg=13;BoxGun.eta[0]=-0.1;BoxGun.eta[1]=0.1;BoxGun.number=1;BoxGun.prange[0]=100.0;BoxGun.prange[1]=101.0;BoxGun.phirange[0]=45.0;BoxGun.phirange[1]=45.0;GeometryManagerParam.useParallelWorld=true;;"
PYTHIAPP="-g pythia8pp"
o2-sim-serial -m PIPE ITS -n ${1:-$NEVENTS} ${BOXGEN} --run 300000 2>&1 | tee log.out

# o2-sim -n 1 -m PIPE ITS -g pythia8pp --run 526641 > log.out #526641
# o2-sim-digitizer-workflow -b
# o2-its-reco-workflow -b --trackerCA --configKeyValues "ITSVertexerParam.phiCut=0.5;ITSVertexerParam.clusterContributorsCut=3;ITSVertexerParam.tanLambdaCut=0.2"
