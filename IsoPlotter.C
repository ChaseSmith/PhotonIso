#include "TH1F.h"

void drawEnergy(TH1F* isoEnergy){
	TCanvas *tc = new TCanvas();
	gPad->SetLogy();
	isoEnergy->Draw();
}


void analyzeFiles(TChain *all){
	float eT;
	all->SetBranchAddress("et_iso",&eT);

	TH1F* isoEnergy = new TH1F("iso","",200,-5,15);

	for (int i = 0; i < all->GetEntries(); ++i)
	{
		all->GetEntry(i);
		isoEnergy->Fill(eT);
	}
	drawEnergy(isoEnergy);
}

void handleG4File(string name, string extension, int filecount){
	TChain *all = new TChain("ttree");
	string temp;
	for (int i = 0; i < filecount; ++i)
	{
		temp = name+to_string(i)+extension;
		all->Add(temp.c_str());
	}
	analyzeFiles(all);
	delete all;
}

void IsoPlotter(){
	string fileLocation = "";
	string filename = "out";
	string extension = "ClusterTree.root";
	string temp = fileLocation+filename;
	handleG4File(temp,extension,10);
}