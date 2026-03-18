/////////////////////////////////////////////////
//                                             //
//                                             //
//  Author: Seungkyu Ha @ seungkyu.ha@cern.ch  //
//                                             //
//                                             //
/////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include <TROOT.h>
#include <TUnixSystem.h>
#include <TChain.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

#include "./interface/Analysis.h"

using namespace std;

TROOT root ("Plots", "Program for CMS Analysis");

//argc: # of arguments, argv:array for arguments
int main(int argc, char **argv)
{
    printf("The number of options is: %i\n",argc-1);
    
    if(argc<2)
    {
       printf("At least, you have to set 1, 2\n");
       printf("1. Input filelist\n");
       printf("2. Output file\n");
       printf("3. Config file name\n");
       printf("4. SE directory output name\n");
       printf("5. RunPeriod\n");
       printf("6. Max Number Events\n");
       printf("7. Brach List\n");
    
/*       printf("4. Muon Candidate Eta Cut On or Off\n");
       printf("5. Muon Candidate Eta Cut \n");
       printf("6. Charge Opposite Sign On or Off\n");
       printf("7. Dimuon Mass Cut On or Off\n");
       printf("8. Dimuon Mass Cut\n");*/
       return 1;
    }
    
    for (int iopt=0; iopt<argc; iopt++)
    {
       printf("Option %i = %s\n",iopt,argv[iopt]);
    }
    
    ///read input options
    
    //cout << "argc: " << argc << endl;
    
    char *flist = argv[1];
    printf("Input filelist = %s\n", flist);
    
    char *outname = argv[2];
    printf("Output file name = %s\n", outname);
    
    char *confname = argv[3];
    printf("Config file name = %s\n", confname);
    
    char *sedirname = argv[4];
    printf("SE Dir name = %s\n", sedirname);
    
    char *runPeriod = argv[5];
    printf("Run period = %s\n", runPeriod);

    //int *runPeriod = argv[5];
    int maxEvt = std::stoi(argv[6]);
    printf("Max Events = %d\n", maxEvt);

    char *brachList = argv[7];
    printf("Brach List File = %s\n", brachList);

    // Optional: JSON config and XSec files (argv[8], argv[9])
    std::string jsonConfigFile = "";
    std::string jsonXSecFile = "";
    if (argc > 8) {
        jsonConfigFile = argv[8];
        printf("JSON Config File = %s\n", argv[8]);
    }
    if (argc > 9) {
        jsonXSecFile = argv[9];
        printf("JSON XSec File = %s\n", argv[9]);
    }

    //merge files
    FILE *filelist;
    char filename[1000];
    string filelistDir, filelistName, filelistPath;
    
    filelistDir = "./input/";
    
    cout << endl;
    filelistName = argv[1];
    filelistPath = filelistDir + filelistName;
    filelist = fopen(filelistPath.c_str(),"r");
   
    std::vector<double> genentries_pertree;
    
    genentries_pertree.clear();
    
    std::vector<double> entries_pertree;
    
    entries_pertree.clear();
    if(filelist == NULL) {std::cout << "no filelist " << filelistPath<< std::endl; return 0;} 
    /*while(filelist==NULL)
    {
       cout << "File not found, please try again." << endl;
       cout << "Filelist you want to use: " << filelistDir;
       cin >> filelistName;      
       filelistPath = filelistDir + filelistName;
       filelist=fopen(filelistPath.c_str(),"r");
    }*/
    
//    TChain *chgen = new TChain("demo/SSBGenTree");
    //TChain *ch    = new TChain("ssbanalyzer/NanoAODTree"   );
    //TChain *ch    = new TChain("ssbanalyzer/SSBMiniTree"   );
    TChain *ch = new TChain("Events");
    
    cout << endl;
    cout << "start merging file(s)" << endl;
    
    while (fscanf(filelist, "%s", filename) != EOF)
    {
       cout << "adding: " << filename << endl;
       ch->Add(filename, 0);
       entries_pertree.push_back(ch->GetEntries());
    }
    cout << "Total number of events after merging root files: " << ch->GetEntries() << endl;

    // 
    Analysis analysis(ch, filelistName, sedirname, outname, Form("./branchlist/%s", brachList), confname, maxEvt, jsonConfigFile, jsonXSecFile);

    analysis.SetVariables();
    // Execute Loop function //
    analysis.Loop();

/*
   //NanoAODTree *nano = new NanoAODTree(ch, "test");
   ssb_analysis *ssb = new ssb_analysis(ch, confname);
   ssb->SetInputFileName(flist);
   //ssb->SetNanoAODHeader();
   //ssb->SetOutputFileName(outname);
   ssb->SetOutputFileName(outname, sedirname);
//   ssb->SetConfigFile(flist);
   ssb->GetNtupleTotalEvent( ch->GetEntries() );
   ssb->Start();
   ssb->Loop( flist );
   ssb->End();
      
   delete ssb;
   cout << "Delete ssb..." << endl << endl; 
   delete ch;
   cout << "End processing..." << endl << endl; 
       
*/
   return 0;
}

