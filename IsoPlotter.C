#include "TH1D.h"

void drawEnergy(TH1D* isoEnergy){
	TCanvas *tc = new TCanvas();
	gPad->SetLogy();
	isoEnergy->Draw();
}


void analyzeFiles(TChain *all){
	double eT;
	all->SetBranchAddress("et_iso",&eT);

	TH1D* isoEnergy = new TH1D("iso","",200,-2,23);

	for (int i = 0; i < all->GetEntries(); ++i)
	{
		all->GetEntry(i);
		if (eT!=0)
		{
			//cout<<eT<<'\n';
			isoEnergy->Fill(eT);	
		}
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