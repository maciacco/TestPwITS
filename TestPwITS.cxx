bool debugprint = false;

void fillPw();

void TestPwITS(int nsteps = 30, double deltaphi = 0 /* rad */, bool usepwnav = true)
{
   TFile ff("o2sim_geometry.root");
   ff.Get("ccdb_object");
   TFile fout("fout_geom.root", "recreate");

   gGeoManager->CloseGeometry();
   TGeoParallelWorld *pw = nullptr;
   TGeoVolume *top = gGeoManager->GetTopVolume();
   pw = gGeoManager->CreateParallelWorld("priority_sensors");
   fillPw();
   if (debugprint) std::cout << "filled" << std::endl;
   pw->CloseGeometry();
   if (debugprint) std::cout << "closed" << std::endl;
   gGeoManager->SetUseParallelWorldNav(usepwnav);
   if (debugprint) std::cout << "navigate" << std::endl;

   TString cname;
   cname = "cpw";
   TCanvas *c = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(cname);
   if (c) c->cd();
   else   c = new TCanvas(cname, "",800,600);

   TStopwatch timer;
   timer.Start();

   double dir[3]; double theta = TMath::Pi() * 0.5; double phi = TMath::Pi() * 0.5 - deltaphi;
   double start[3]{0, 0, 0};
   dir[0] = TMath::Sin(theta) * TMath::Cos(phi);
   dir[1] = TMath::Sin(theta) * TMath::Sin(phi);
   dir[2] = TMath::Cos(theta);
   if (debugprint) std::cout << "init track" << std::endl;
   auto n = gGeoManager->InitTrack(start[0], start[1], start[2], dir[0], dir[1], dir[2]);
   if (debugprint) std::cout << "init'ed track" << std::endl;

   std::cout << "\n+++++++++++++++++++++++++++++" << std::endl;
   std::cout << "Init volume = " << n->GetVolume()->GetName() << std::endl;
   std::cout << "+++++++++++++++++++++++++++++\n" << std::endl;

   TPolyLine3D *line = new TPolyLine3D(nsteps);
   line->SetLineColor(kRed);
   line->SetLineWidth(2);
   int ipoint = 0;
   line->SetPoint(0, start[0], start[1], start[2]);
   for (int i = 0; i < nsteps; ++i) {
      if (debugprint) std::cout << "get next boundary" << std::endl;
      n = gGeoManager->FindNextBoundaryAndStep();
      if (debugprint) std::cout << "got next boundary" << std::endl;
      const double *p = gGeoManager->GetCurrentPoint();
      std::cout << "\n+++++++++++++++++++++++++++++" << std::endl;
      std::cout << "Next volume: " << n->GetVolume()->GetName() << std::endl;
      std::cout << "x = " << p[0] << "\ty = " << p[1] << "\tz = " << p[2] << std::endl;
      std::cout << "+++++++++++++++++++++++++++++\n" << std::endl;
      line->SetNextPoint(p[0], p[1], p[2]);
   }

   TGeoVolume *v = pw->GetVolume();
   v->Draw(0);
   if (debugprint) {
      std::cout << "lastPoint = " << line->GetLastPoint() << std::endl;
      line->Print();
   }
   line->Draw("same");

   fout.cd();
   c->Write();
   timer.Stop();
   timer.Print();
   pw->PrintDetectedOverlaps();
   fout.Close();
}

void fillPw()
{
   TGeoPhysicalNode *node;
   TGeoParallelWorld *pw = gGeoManager->GetParallelWorld();
   for (int l=0; l < 1; l++) { // layer
      for (int k=0; k < 1; k++) {  // half barrel
         for (int j=0; j < 6; j++) { // stave
            for (int i=4; i<5; i++) { // chip
               node = gGeoManager->MakePhysicalNode(Form("/cave/barrel_1/ITSV_2/ITSUWrapVol0_1/ITSULayer%d_1/ITSUHalfBarrel%d_%d/ITSUStave%d_%d/ITSUChip%d_%d", l, l, k, l, j, l, i));
               if (pw) {
                  pw->AddNode(Form("/cave/barrel_1/ITSV_2/ITSUWrapVol0_1/ITSULayer%d_1/ITSUHalfBarrel%d_%d/ITSUStave%d_%d/ITSUChip%d_%d/ITSUSensor0_1", l, l, k, l, j, l, i));
                  pw->AddNode(Form("/cave/barrel_1/ITSV_2/ITSUWrapVol0_1/ITSULayer%d_1/ITSUHalfBarrel%d_%d/ITSUStave%d_%d/ITSUChip%d_%d/MetalStack_1", l, l, k, l, j, l, i));
               }
            }
         }
      }
   }
}

// this version reproduces the infinite loop bug
/* void fillPw()
{
   TGeoPhysicalNode *node;
   TGeoParallelWorld *pw = gGeoManager->GetParallelWorld();
   for (int l=0; l < 1; l++) { // layer
      for (int k=0; k < 1; k++) {  // half barrel
         for (int j=0; j < 6; j++) { // stave
            for (int i=4; i<5; i++) { // chip
               node = gGeoManager->MakePhysicalNode(Form("/cave/barrel_1/ITSV_2/ITSUWrapVol0_1/ITSULayer%d_1/ITSUHalfBarrel%d_%d/ITSUStave%d_%d/ITSUChip%d_%d", l, l, k, l, j, l, i));
               if (pw) pw->AddNode(Form("/cave/barrel_1/ITSV_2/ITSUWrapVol0_1/ITSULayer%d_1/ITSUHalfBarrel%d_%d/ITSUStave%d_%d/ITSUChip%d_%d", l, l, k, l, j, l, i));
            }
         }
      }
   }
}
*/
