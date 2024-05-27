#include <vector>
#include <numbers>

#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>

#include <ITSMFTSimulation/Hit.h>

constexpr float layerMinRad[]{2.24f, 3.01f, 3.78f, 19.44f, 24.39f, 34.23f, 39.18f};
constexpr float layerMaxRad[]{2.67f, 3.46f, 4.21f, 19.77f, 24.70f, 34.54f, 39.49f};
constexpr float layerHalfLength[]{14.f, 14.f, 14.f, 43.f, 43.f, 75.f, 75.f};
constexpr float epsilon[]{.1f, .1f, .1f, 1.f, 1.f, 1.f, 1.f};

int getLayer(const float r)
{
  for (int iL{0}; iL < 7; ++iL) {
    if (r > layerMinRad[iL] - epsilon[iL] && r < layerMaxRad[iL] + epsilon[iL]) {
      return iL;
    }
  }
  return -999;
}

void CheckHits()
{
  gStyle->SetOptStat(0);
  TFile* file = TFile::Open("o2sim_HitsITS.root");
  TTree* hitTree = (TTree*)file->Get("o2sim");
  std::vector<o2::itsmft::Hit>* hitArr = nullptr;
  hitTree->SetBranchAddress("ITSHit", &hitArr);

  TH2F* xy = new TH2F("xy", ";x (cm); y (cm)", 4000, -50., 50., 4000, -50., 50.);

  TH2F* etaPhi[7];
  for (int iL{0}; iL < 7; ++iL) {
    etaPhi[iL] = new TH2F(Form("zPhi_L%d", iL), Form("Layer %d;#phi (rad);z (cm)", iL), 130, 0., 6.5, 100, -layerHalfLength[iL], layerHalfLength[iL]);
  }
  uint64_t nEv = hitTree->GetEntriesFast();
  for (uint64_t iE{0}; iE < nEv; ++iE) {
    hitTree->GetEvent(iE);
    for (unsigned int iHit{0}; iHit < hitArr->size(); ++iHit) {
      auto hit = hitArr->at(iHit);
      int layer = getLayer(std::hypot(hit.GetX(), hit.GetY()));
      if (layer < 0) continue;
      etaPhi[layer]->Fill(std::atan2(hit.GetY(), hit.GetX()) + std::numbers::pi, hit.GetZ());
      xy->Fill(hit.GetX(), hit.GetY());
    }
  }

  TFile* out = new TFile("CheckHits.root", "recreate");
  TCanvas cPlot("plotLayers", "plotLayers");
  cPlot.Divide(2, 4);
  out->cd();
  for (int iL{0}; iL < 7; ++iL) {
    etaPhi[iL]->Write();
    cPlot.cd(iL + 1);
    cPlot.SetRightMargin(0.18);
    etaPhi[iL]->Draw("colz");
  }
  cPlot.Write();
  xy->Write();
  out->Close();
  file->Close();
}
