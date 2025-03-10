int PDGtoIndex(int pID)
{
	enum {pro,neu,ele,ion,ELSE};
	int idx=0;
	if(pID == 2212)       idx = pro;
	else if (pID == 2112) idx = neu;
	else if (pID == 11 || pID == -11) idx = ele;
	else if (pID > 1000000000) idx = ion;
	else idx = ELSE;
	return idx;
}

void nonOpMake(
		const char* particle = "temp",
		const char* energy = "",
		const char* suffix = "",
		const char* d_pref = "./data_root")
{
	/* HIST lists
		 0. 1-D Trig cond (1&2, 2&3, 3&4)
		 1. 2-D[SCs] incident particle - nStep at each SC
		 2. 1-D[SCs][p,n,e,I,else] Edep sum with track tagging
		 3. 2-D[SCs-1][] delta energy weighted T - avg edep
	*/

	const int n_Hist = 1;
	bool Opt[n_Hist]; fill_n(Opt,n_Hist,1);
	const int StepFromHit = 1;

	char* infile;
	TString str_p = particle;
	TString str_s = suffix;
	if(str_p.Contains("temp")){
		if(str_s=="") infile = Form("out_%s.root",particle);
		else          infile = Form("out_%s_%s.root",particle,suffix);
	}else{
		if(str_s=="") infile = Form("out_%s_%s.root",particle,energy);
		else          infile = Form("out_%s_%s_%s.root",particle,energy,suffix);
	}

	cout << "#################" << endl;
	cout << "infile: " << infile << endl;
	cout << "#################" << endl;

	// load SiPM efficiency csv file
	ifstream in("SiPMEff.csv");
	string line;
	getline(in,line); // dummy line (title)
	vector<double> vec_wav,vec_eff,vec_E;
	while(getline(in,line))
	{
		stringstream ss(line);
		double wavlen, E, eff;
		ss >> wavlen >> E >> eff;
		vec_wav.push_back(wavlen);
		vec_E.push_back(eff);
		vec_eff.push_back(E);
	}
	in.close();

	// load the data root file
	TFile* F = new TFile(Form("%s/%s",d_pref,infile),"read");
	TList* L_para = (TList*) F -> Get("InputParameters");
	TTree* T = (TTree*) F -> Get("Opsim");
	map<string,string> map_para;
	for(int a=0; a<L_para->GetSize(); a++)
	{
		TNamed* name = (TNamed*) L_para->At(a);
		map_para.insert(make_pair(name->GetName(),name->GetTitle()));
//		cout << name -> GetName() << " " << name -> GetTitle() << endl;
	}

	// Define container for data
	const int max = 10000;

	int nTrack;
	int t_ID[max], t_ProcID[max], t_ParentID[max], t_PDG[max], t_DetID[max];
	double t_px[max], t_py[max], t_pz[max], t_vx[max], t_vy[max], t_vz[max];
	double t_time[max], t_KE[max];

	int nPostTrack;
	int p_ID[max], p_ProcID[max], p_DetID[max];
	double p_px[max], p_py[max], p_pz[max], p_vx[max], p_vy[max], p_vz[max];
	double p_time[max], p_KE[max];

	int nStep;
	int s_ID[max], s_FromHit[max], s_ProcID[max], s_PDG[max], s_DetID[max];
	double s_vx[max], s_vy[max], s_vz[max], s_time[max], s_Edep[max];
	double s_prevKE[max];


	if(map_para["MCTrack"] == "true")
	{
		cout << "MCTrack was called" << endl;
		T -> SetBranchAddress("nTrack",&nTrack);
		T -> SetBranchAddress("TrackID",t_ID);
		T -> SetBranchAddress("TrackProcID",t_ProcID);
		T -> SetBranchAddress("ParentID",t_ParentID);
		T -> SetBranchAddress("TrackPDG",t_PDG);
		T -> SetBranchAddress("TrackDetID",t_DetID);
		T -> SetBranchAddress("TrackPX",t_px);
		T -> SetBranchAddress("TrackPY",t_py);
		T -> SetBranchAddress("TrackPZ",t_pz);
		T -> SetBranchAddress("TrackVX",t_vx);
		T -> SetBranchAddress("TrackVY",t_vy);
		T -> SetBranchAddress("TrackVZ",t_vz);
		T -> SetBranchAddress("TrackTime",t_time);
		T -> SetBranchAddress("TrackKEnergy",t_KE);
	}
	if(map_para["MCPostTrack"] == "true")
	{
		cout << "MCPostTrack was called" << endl;
		T -> SetBranchAddress("nPostTrack",&nPostTrack);
		T -> SetBranchAddress("PostTrackID",p_ID);
		T -> SetBranchAddress("PostTrackProcID",p_ProcID);
		T -> SetBranchAddress("PostTrackDetID",p_DetID);
		T -> SetBranchAddress("PostTrackPX",p_px);
		T -> SetBranchAddress("PostTrackPY",p_py);
		T -> SetBranchAddress("PostTrackPZ",p_pz);
		T -> SetBranchAddress("PostTrackVX",p_vx);
		T -> SetBranchAddress("PostTrackVY",p_vy);
		T -> SetBranchAddress("PostTrackVZ",p_vz);
		T -> SetBranchAddress("PostTrackTime",p_time);
		T -> SetBranchAddress("PostTrackKEnergy",p_KE);
	}
	if(map_para["MCStep"] == "true")
	{
		cout << "MCStep was called" << endl;
		T -> SetBranchAddress("nStep",&nStep);
		T -> SetBranchAddress("StepTrackID",s_ID); 
		T -> SetBranchAddress("StepFromHit",s_FromHit);
		T -> SetBranchAddress("StepProcID",s_ProcID);
		T -> SetBranchAddress("StepTrackPDG",s_PDG); 
		T -> SetBranchAddress("StepPrevDetID",s_DetID);
		T -> SetBranchAddress("StepVX",s_vx);
		T -> SetBranchAddress("StepVY",s_vy);
		T -> SetBranchAddress("StepVZ",s_vz);
		T -> SetBranchAddress("StepTime",s_time);
		T -> SetBranchAddress("StepEdep",s_Edep);
		T -> SetBranchAddress("StepPrevKE",s_prevKE);
	}

	enum {SC1,SC2,SC3,SC4};
	char* str_SC[] = {"SC1","SC2","SC3","SC4"};
	const int nSC = sizeof(str_SC)/sizeof(str_SC[0]);
	enum {pro,neu,ele,ion};
	char* str_pID[] = {"pro","neu","ele","ion","ELSE"};
	const int npID = sizeof(str_pID)/sizeof(str_pID[0]);

	TH1D* H1_Trig = new TH1D("H1_Trig","",3,0,3); // HIST 0
	TH2F* H2_Beam_nStep[nSC];                     // HIST 1
	TH1F* H1_EdepSum[nSC][npID];                  // HIST 2
	TH2F* H2_Time_Edep[nSC-1][npID];              // HIST 3

	for(int a=0; a<nSC; a++)
	{
		H2_Beam_nStep[a] = 
			new TH2F(Form("H2_Beam_nSteps_%s",str_SC[a]),"",npID,0,npID,100,0,100);
		for(int b=0; b<npID; b++)
		{
			H1_EdepSum[a][b] = 
				new TH1F(Form("H1_Edep_%s_%s",str_SC[a],str_pID[b]),"",200,0,10);
		}
	}
	for(int a=0; a<nSC-1; a++)
	for(int b=0; b<npID; b++)
	{

		H2_Time_Edep[a][b] = 
			new TH2F(Form("H2_Time_Edep_%s_%s_%s",str_SC[a],str_SC[a+1],str_pID[b]),"",
					6000,-10,50,400,0,20);
	}

	// event loop
	for(int a=0; a<T->GetEntries(); a++)
	{
		if(a%100 == 0)
			cout << "Processing " << a << " th event" << endl;
		T -> GetEntry(a);
		int pri_pdg = 0;

		for(int b=0; b<nTrack; b++)
		{
			int pdg = t_PDG[b];
			int parentID = t_ParentID[b];
			if(parentID==0)
				pri_pdg = PDGtoIndex(pdg);
		}

		int nStepSC[nSC];    fill_n(nStepSC,nSC,0);
		double EdepSum[nSC]; fill_n(EdepSum,nSC,0);
		double EwTime[nSC];  fill_n(EwTime,nSC,0);
		bool b_SCs[nSC];     fill_n(b_SCs,nSC,0);
		for(int b=0; b<nStep; b++)
		{
			int fromHit = s_FromHit[b];
			if(fromHit == 1) continue;
			int proc = s_ProcID[b];
			int detID = s_DetID[b] - stoi(map_para["SCID"]) -1;
			double energy = s_prevKE[b];
			double edep = s_Edep[b];
			double time = s_time[b];
			double vx = s_vx[b];
			double vy = s_vy[b];
			if(detID <0 || detID > 10) continue;
			if(detID >= 0 && edep > 0)
			{
				nStepSC[detID]++;
				EdepSum[detID] += edep;
				EwTime[detID] += edep*time;
			}
		}

		for(int b=0; b<nSC; b++)
		{
			if(EdepSum[b] !=0)
			{
				b_SCs[b] = true;
				EwTime[b] = EwTime[b]/EdepSum[b];
			}
			else
			{
				b_SCs[b] = false;
				EwTime[b] = -1;
			}
			// HIST 1
			H2_Beam_nStep[b] -> Fill(pri_pdg,nStepSC[b]);
			// HIST 2
			H1_EdepSum[b][pri_pdg] -> Fill(EdepSum[b]);
		}
		// HIST 0
		if(b_SCs[SC1] && b_SCs[SC2])
			H1_Trig -> Fill(0.5);
		if(b_SCs[SC2] && b_SCs[SC3])
			H1_Trig -> Fill(1.5);
		if(b_SCs[SC3] && b_SCs[SC4])
			H1_Trig -> Fill(2.5);

		// HIST 3
		for(int b=0; b<nSC-1; b++)
		{
			if(b_SCs[b] && b_SCs[b+1])
				H2_Time_Edep[b][pri_pdg] -> Fill(EwTime[b+1] - EwTime[b],0.5*EdepSum[b]+0.5*EdepSum[b+1]);
		}
	}

	// output root file
	char* output_prefix = "./out_root";
	TFile* G = new TFile(Form("%s/H_%s",output_prefix,infile),"recreate");

	// Write
	H1_Trig -> Write();
	for(int a=0; a<nSC; a++)
	{
		H2_Beam_nStep[a] -> Write();
		for(int b=0; b<npID; b++)
		{
			H1_EdepSum[a][b] -> Write();
		}
	}
	for(int a=0; a<nSC-1; a++)
	for(int b=0; b<npID; b++)
	{
			H2_Time_Edep[a][b] -> Write();
	}
	F -> Close();
	G -> Close();
}
