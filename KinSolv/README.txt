I am sharing with you codes from 8 TeV, as I have them ready for sharing. This shouldn't be a problem, becuase the kin. reco. algorithm was not changed in any way. However, the following items have to be considered.

1) There are 2 places in codes, where you can set "TopMASS", i.e. top quark mass value used for constraint. So, if you want to change it, you have to do it in 2 places.
2) There are few hardcoded values for "btag_wp", i.e., b-tag working point.
3) 
In KinematicReconstruction.cc you have to update to a new .root input file "KinReco_input_2016_Run2BtoH_25ns_80X_v037_dilep_sel_25Sep2017_NLODY_FineEleID.root", if you want to use same resolutions as in TOP-17-014. This file is included in the tarball.
4)
For the selection used in TOP-17-014, the following kinematic reconstruction scale factors were estimated:
    else if(era_ == Era::run2_13tev_2016_25ns) {
        // SF for 80X (35922pb)
        m_sfNomEE = 1.0070; m_sfNomEMu = 1.0010; m_sfNomMuMu = 0.9993;
        m_sfUncEE = 0.0027; m_sfUncEMu = 0.0011; m_sfUncMuMu = 0.0019;
    }

You have to implement these scale factors in KinematicReconstruction.cc .

5) As long as you are using the same object definition and event selection as in TOP-17-014, it should be safe for you to use our inputs and scale factors. Otherwise, you have to reestimate them.


