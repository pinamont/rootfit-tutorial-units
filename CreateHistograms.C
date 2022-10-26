#include "TRandom.h"
#include "TFile.h"
#include "TH1F.h"
#include "TSystem.h"

void CreateHistograms(){
    gSystem->mkdir("ExampleInputs");
    
    double bins[] = {0,100,200,300,400};
    int nbins = 4;
    
    double k_lumi = 1.;
    double k_sig = 10;
    
    //
    // create signal histograms
    //
    TFile *f_sig = new TFile("ExampleInputs/sig.root","RECREATE");
    //
    // Nominal
    //
    // SR
    TH1F *h_sig = new TH1F("h_sig","Signal",nbins,bins);
    float yield_sig[] = {0.1,0.5,1.5,3.1};
    for(int i_bin=1;i_bin<=sizeof(yield_sig)/sizeof(float);i_bin++){
        h_sig->SetBinContent(i_bin,k_sig*k_lumi*yield_sig[i_bin-1]);
    }
    h_sig->Write("HTj",TObject::kOverwrite);
    // CR
    TH1F *h_sig_CR = (TH1F*) h_sig->Clone("HTj_CR");
    h_sig_CR->Scale(0.1);
    h_sig_CR->Write("HTj_CR",TObject::kOverwrite);
    //
    // Systematic
    //
    // SR
    TH1F *h_sig_jesUp = new TH1F("h_sig_jesUp","Signal jesUp",nbins,bins);
    float yield_sig_jesUp[] = {0.11,0.51,1.52,3.13};
    for(int i_bin=1;i_bin<=sizeof(yield_sig_jesUp)/sizeof(float);i_bin++){
      h_sig_jesUp->SetBinContent(i_bin,k_sig*k_lumi*yield_sig_jesUp[i_bin-1]);
    }
    h_sig_jesUp->Write("HTj_jesUp",TObject::kOverwrite);
    // CR
    TH1F *h_sig_jesUp_CR = (TH1F*) h_sig_jesUp->Clone("h_sig_jesUp_CR");
    h_sig_jesUp_CR->Scale(0.1);
    h_sig_jesUp->Write("HTj_CR_jesUp",TObject::kOverwrite);
    //
    // create the down variations as specular wrt the postive ones
    TH1F *h_sig_jesDown = new TH1F("h_sig_jesDown","Signal jesDown",nbins,bins);
    h_sig_jesDown->Add(h_sig,2);
    h_sig_jesDown->Add(h_sig_jesUp,-1);
    h_sig_jesDown->Write("HTj_jesDown",TObject::kOverwrite);
    TH1F *h_sig_jesDown_CR = (TH1F*) h_sig_jesDown->Clone("h_sig_jesDown_CR");
    h_sig_jesDown_CR->Scale(0.1);
    h_sig_jesDown_CR->Write("HTj_CR_jesDown",TObject::kOverwrite);

    //
    // create bkg1 histograms
    //
    TFile *f_bkg1 = new TFile("ExampleInputs/bkg1.root","RECREATE");
    //
    // Nominal
    //
    // SR
    TH1F *h_bkg1 = new TH1F("h_bkg1","Background1",nbins,bins);
    float yield_bkg1[] = {100.,75.,20.,10.};
    float stat_bkg1[] = {1.,1.,2.,0.5};
    for(int i_bin=1;i_bin<=sizeof(yield_bkg1)/sizeof(float);i_bin++){
        h_bkg1->SetBinContent(i_bin,k_lumi*yield_bkg1[i_bin-1]);
        h_bkg1->SetBinError(i_bin,stat_bkg1[i_bin-1]);
    }
    h_bkg1->Write("HTj",TObject::kOverwrite);
    // CR
    TH1F *h_bkg1_CR = (TH1F*) h_bkg1->Clone("HTj_CR");
    h_bkg1_CR->Scale(3.);
    h_bkg1_CR->Write("HTj_CR",TObject::kOverwrite);
    //
    // Systematic
    //
    // SR
    TH1F *h_bkg1_jesUp = new TH1F("h_bkg1_jesUp","Background1 jesUp",nbins,bins);
    float yield_bkg1_jesUp[] = {90.,70.,30.,30.};
    for(int i_bin=1;i_bin<=sizeof(yield_bkg1_jesUp)/sizeof(float);i_bin++){
        h_bkg1_jesUp->SetBinContent(i_bin,k_lumi*yield_bkg1_jesUp[i_bin-1]);
        h_bkg1_jesUp->SetBinError(i_bin,stat_bkg1[i_bin-1]);
    }
    h_bkg1_jesUp->Write("HTj_jesUp",TObject::kOverwrite);
    // CR
    TH1F *h_bkg1_jesUp_CR = (TH1F*) h_bkg1_jesUp->Clone("h_bkg1_CR_jesUp");
    h_bkg1_jesUp_CR->Scale(3.);
    h_bkg1_jesUp_CR->Write("HTj_CR_jesUp",TObject::kOverwrite);
    //
    // create the down variations as specular wrt the postive ones
    TH1F *h_bkg1_jesDown = new TH1F("h_bkg1_jesDown","Background1 jesDown",nbins,bins);
    h_bkg1_jesDown->Add(h_bkg1,2);
    h_bkg1_jesDown->Add(h_bkg1_jesUp,-1);
    h_bkg1_jesDown->Write("HTj_jesDown",TObject::kOverwrite);
    TH1F *h_bkg1_jesDown_CR = (TH1F*) h_bkg1_jesDown->Clone("h_bkg1_CR_jesDown");
    h_bkg1_jesDown_CR->Scale(3.);
    h_bkg1_jesDown_CR->Write("HTj_CR_jesDown",TObject::kOverwrite);
  
    //
    // create bkg2 histograms
    //
    TFile *f_bkg2 = new TFile("ExampleInputs/bkg2.root","RECREATE");
    //
    // Nominal
    //
    // SR
    TH1F *h_bkg2 = new TH1F("h_bkg2","Background2",nbins,bins);
    float yield_bkg2[] = {10.,30.,50.,15.};
    for(int i_bin=1;i_bin<=sizeof(yield_bkg2)/sizeof(float);i_bin++){
        h_bkg2->SetBinContent(i_bin,k_lumi*yield_bkg2[i_bin-1]);
    }
    h_bkg2->Write("HTj",TObject::kOverwrite);
    // CR
    TH1F *h_bkg2_CR = (TH1F*) h_bkg2->Clone("HTj_CR");
    h_bkg2_CR->Scale(2.);
    h_bkg2_CR->Write("HTj_CR",TObject::kOverwrite);
    
    //
    // create data histograms
    //
    TFile *f_data = new TFile("ExampleInputs/data.root","RECREATE");
    //
    // SR
    TH1F *h_data = new TH1F("h_data","Data",nbins,bins);
    for(int i_bin=1;i_bin<=nbins;i_bin++){
        // const double prediction =  k_sig*yield_sig[i_bin-1] + yield_bkg1[i_bin-1] + yield_bkg2[i_bin-1];
        double prediction = 0.;
        prediction += h_sig->GetBinContent(i_bin);
        prediction += h_bkg1->GetBinContent(i_bin);
        prediction += h_bkg2->GetBinContent(i_bin);
        const int yield_data = gRandom->Poisson( prediction );
        h_data->SetBinContent(i_bin, yield_data);
    }
    h_data->Write("HTj",TObject::kOverwrite);
    //
    // CR
    TH1F *h_data_CR = new TH1F("h_data_CR","Data",nbins,bins);
    for(int i_bin=1;i_bin<=nbins;i_bin++){
        // const double prediction =  k_sig*yield_sig[i_bin-1]*0.1 + yield_bkg1[i_bin-1]*3.0 + yield_bkg2[i_bin-1]*2.0;
        double prediction = 0.;
        prediction += h_sig_CR->GetBinContent(i_bin);
        prediction += h_bkg1_CR->GetBinContent(i_bin);
        prediction += h_bkg2_CR->GetBinContent(i_bin);
        const int yield_data = gRandom->Poisson( prediction );
        h_data_CR->SetBinContent(i_bin, yield_data);
    }
    h_data_CR->Write("HTj_CR",TObject::kOverwrite);
  
  
    // Test: plotting
    h_sig->SetFillColor(kRed);
    h_bkg1->SetFillColor(kYellow);
    h_bkg2->SetFillColor(kGreen);
    TCanvas *c = new TCanvas("c","c",600,600);
    THStack *h_tot = new THStack();
    h_tot->Add(h_bkg2);
    h_tot->Add(h_bkg1);
    h_tot->Add(h_sig);
    h_tot->Draw("HIST");
    h_data->Draw("same E");
    
    c->SaveAs("Histos.png");
  
}
