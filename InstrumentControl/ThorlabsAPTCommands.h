#pragma once

#define MGMSG_HUB_GET_BAYUSED 0x0066
#define MGMSG_HUB_REQ_BAYUSED 0x0065
#define MGMSG_HW_DISCONNECT 0x0002
#define MGMSG_HW_GET_INFO 0x0006
#define MGMSG_HW_NO_FLASH_PROGRAMMING 0x0018
#define MGMSG_HW_REQ_INFO 0x0005
#define MGMSG_HW_RESPONSE 0x0080
#define MGMSG_HW_RICHRESPONSE 0x0081
#define MGMSG_HW_START_UPDATEMSGS 0x0011
#define MGMSG_HW_STOP_UPDATEMSGS 0x0012
#define MGMSG_HW_YES_FLASH_PROGRAMMING 0x0017
#define MGMSG_LA_ACK_STATUSUPDATE 0x0822
#define MGMSG_LA_DISABLEOUTPUT 0x0812
#define MGMSG_LA_ENABLEOUTPUT 0x0811
#define MGMSG_LA_GET_PARAMS 0x0802
#define MGMSG_LA_GET_STATUSUPDATE 0x0821
#define MGMSG_LA_REQ_PARAMS 0x0801
#define MGMSG_LA_REQ_STATUSUPDATE 0x0820
#define MGMSG_LA_SET_EEPROMPARAMS 0x0810
#define MGMSG_LA_SET_PARAMS 0x0800
#define MGMSG_MOD_GET_CHANENABLESTATE 0x0212
#define MGMSG_MOD_GET_DIGOUTPUTS 0x0215
#define MGMSG_MOD_IDENTIFY 0x0223
#define MGMSG_MOD_REQ_CHANENABLESTATE 0x0211
#define MGMSG_MOD_REQ_DIGOUTPUTS 0x0214
#define MGMSG_MOD_SET_CHANENABLESTATE 0x0210
#define MGMSG_MOD_SET_DIGOUTPUTS 0x0213
#define MGMSG_MOT_ACK_DCSTATUSUPDATE 0x0492
#define MGMSG_MOT_GET_ADCINPUTS 0x042C
#define MGMSG_MOT_GET_AVMODES 0x04B5
#define MGMSG_MOT_GET_BOWINDEX 0x04F6
#define MGMSG_MOT_GET_BUTTONPARAMS 0x04B8
#define MGMSG_MOT_GET_DCPIDPARAMS 0x04A2
#define MGMSG_MOT_GET_DCSTATUSUPDATE 0x0491
#define MGMSG_MOT_GET_ENCCOUNTER 0x040B
#define MGMSG_MOT_GET_GENMOVEPARAMS 0x043C
#define MGMSG_MOT_GET_HOMEPARAMS 0x0442
#define MGMSG_MOT_GET_JOGPARAMS 0x0418
#define MGMSG_MOT_GET_LIMSWITCHPARAMS 0x0425
#define MGMSG_MOT_GET_MFF_OPERPARAMS 0x0512
#define MGMSG_MOT_GET_MOVEABSPARAMS 0x0452
#define MGMSG_MOT_GET_MOVERELPARAMS 0x0447
#define MGMSG_MOT_GET_PMDCURRENTLOOPPARAMS 0x04D6
#define MGMSG_MOT_GET_PMDJOYSTICKPARAMS 0x04E8
#define MGMSG_MOT_GET_PMDMOTOROUTPUTPARAMS 0x04DC
#define MGMSG_MOT_GET_PMDPOSITIONLOOPPARAMS 0x04D9
#define MGMSG_MOT_GET_PMDPROFILEMODEPARAMS 0x04E5
#define MGMSG_MOT_GET_PMDSETTLEDCURRENTLOOPPARAMS 0x04EB
#define MGMSG_MOT_GET_PMDSTAGEAXISPARAMS 0x04F2
#define MGMSG_MOT_GET_PMDTRACKSETTLEPARAMS 0x04E2
#define MGMSG_MOT_GET_POSCOUNTER 0x0412
#define MGMSG_MOT_GET_POTPARAMS 0x04B2
#define MGMSG_MOT_GET_POWERPARAMS 0x0428
#define MGMSG_MOT_GET_SOL_CYCLEPARAMS 0x04C5
#define MGMSG_MOT_GET_SOL_INTERLOCKMODE 0x04C8
#define MGMSG_MOT_GET_SOL_OPERATINGMODE 0x04C2
#define MGMSG_MOT_GET_SOL_STATE 0x04CD
#define MGMSG_MOT_GET_STATUSBITS 0x042A
#define MGMSG_MOT_GET_STATUSUPDATE 0x0481
#define MGMSG_MOT_GET_TRIGGER 0x0502
#define MGMSG_MOT_GET_VELPARAMS 0x0415
#define MGMSG_MOT_MOVE_ABSOLUTE 0x0453
#define MGMSG_MOT_MOVE_COMPLETED 0x0464
#define MGMSG_MOT_MOVE_HOME 0x0443
#define MGMSG_MOT_MOVE_HOMED 0x0444
#define MGMSG_MOT_MOVE_JOG 0x046A
#define MGMSG_MOT_MOVE_RELATIVE 0x0448
#define MGMSG_MOT_MOVE_STOP 0x0465
#define MGMSG_MOT_MOVE_STOPPED 0x0466
#define MGMSG_MOT_MOVE_VELOCITY 0x0457
#define MGMSG_MOT_REQ_ADCINPUTS 0x042B
#define MGMSG_MOT_REQ_AVMODES 0x04B4
#define MGMSG_MOT_REQ_BOWINDEX 0x04F5
#define MGMSG_MOT_REQ_BUTTONPARAMS 0x04B7
#define MGMSG_MOT_REQ_DCPIDPARAMS 0x04A1
#define MGMSG_MOT_REQ_DCSTATUSUPDATE 0x0490
#define MGMSG_MOT_REQ_ENCCOUNTER 0x040A
#define MGMSG_MOT_REQ_GENMOVEPARAMS 0x043B
#define MGMSG_MOT_REQ_HOMEPARAMS 0x0441
#define MGMSG_MOT_REQ_JOGPARAMS 0x0417
#define MGMSG_MOT_REQ_LIMSWITCHPARAMS 0x0424
#define MGMSG_MOT_REQ_MFF_OPERPARAMS 0x0511
#define MGMSG_MOT_REQ_MOVEABSPARAMS 0x0451
#define MGMSG_MOT_REQ_MOVERELPARAMS 0x0446
#define MGMSG_MOT_REQ_PMDCURRENTLOOPPARAMS 0x04D5
#define MGMSG_MOT_REQ_PMDJOYSTICKPARAMS 0x04E7
#define MGMSG_MOT_REQ_PMDMOTOROUTPUTPARAMS 0x04DB
#define MGMSG_MOT_REQ_PMDPOSITIONLOOPPARAMS 0x04D8
#define MGMSG_MOT_REQ_PMDPROFILEMODEPARAMS 0x04E4
#define MGMSG_MOT_REQ_PMDSETTLEDCURRENTLOOPPARAMS 0x04EA
#define MGMSG_MOT_REQ_PMDSTAGEAXISPARAMS 0x04F1
#define MGMSG_MOT_REQ_PMDTRACKSETTLEPARAMS 0x04E1
#define MGMSG_MOT_REQ_POSCOUNTER 0x0411
#define MGMSG_MOT_REQ_POTPARAMS 0x04B1
#define MGMSG_MOT_REQ_POWERPARAMS 0x0427
#define MGMSG_MOT_REQ_SOL_CYCLEPARAMS 0x04C4
#define MGMSG_MOT_REQ_SOL_INTERLOCKMODE 0x04C7
#define MGMSG_MOT_REQ_SOL_OPERATINGMODE 0x04C1
#define MGMSG_MOT_REQ_SOL_STATE 0x04CC
#define MGMSG_MOT_REQ_STATUSBITS 0x0429
#define MGMSG_MOT_REQ_STATUSUPDATE 0x0480
#define MGMSG_MOT_REQ_TRIGGER 0x0501
#define MGMSG_MOT_REQ_VELPARAMS 0x0414
#define MGMSG_MOT_RESUME_ENDOFMOVEMSGS 0x046C
#define MGMSG_MOT_SET_AVMODES 0x04B3
#define MGMSG_MOT_SET_BOWINDEX 0x04F4
#define MGMSG_MOT_SET_BUTTONPARAMS 0x04B6
#define MGMSG_MOT_SET_DCPIDPARAMS 0x04A0
#define MGMSG_MOT_SET_EEPROMPARAMS 0x04B9
#define MGMSG_MOT_SET_ENCCOUNTER 0x0409
#define MGMSG_MOT_SET_GENMOVEPARAMS 0x043A
#define MGMSG_MOT_SET_HOMEPARAMS 0x0440
#define MGMSG_MOT_SET_JOGPARAMS 0x0416
#define MGMSG_MOT_SET_LIMSWITCHPARAMS 0x0423
#define MGMSG_MOT_SET_MFF_OPERPARAMS 0x0510
#define MGMSG_MOT_SET_MOVEABSPARAMS 0x0450
#define MGMSG_MOT_SET_MOVERELPARAMS 0x0445
#define MGMSG_MOT_SET_PMDCURRENTLOOPPARAMS 0x04D4
#define MGMSG_MOT_SET_PMDJOYSTICKPARAMS 0x04E6
#define MGMSG_MOT_SET_PMDMOTOROUTPUTPARAMS 0x04DA
#define MGMSG_MOT_SET_PMDPOSITIONLOOPPARAMS 0x04D7
#define MGMSG_MOT_SET_PMDPROFILEMODEPARAMS 0x04E3
#define MGMSG_MOT_SET_PMDSETTLEDCURRENTLOOPPARAMS 0x04E9
#define MGMSG_MOT_SET_PMDSTAGEAXISPARAMS 0x04F0
#define MGMSG_MOT_SET_PMDTRACKSETTLEPARAMS 0x04E0
#define MGMSG_MOT_SET_POSCOUNTER 0x0410
#define MGMSG_MOT_SET_POTPARAMS 0x04B0
#define MGMSG_MOT_SET_POWERPARAMS 0x0426
#define MGMSG_MOT_SET_PZSTAGEPARAMDEFAULTS 0x0686
#define MGMSG_MOT_SET_SOL_CYCLEPARAMS 0x04C3
#define MGMSG_MOT_SET_SOL_INTERLOCKMODE 0x04C6
#define MGMSG_MOT_SET_SOL_OPERATINGMODE 0x04C0
#define MGMSG_MOT_SET_SOL_STATE 0x04CB
#define MGMSG_MOT_SET_TRIGGER 0x0500
#define MGMSG_MOT_SET_TSTACTUATORTYPE 0x04FE
#define MGMSG_MOT_SET_VELPARAMS 0x0413
#define MGMSG_MOT_SUSPEND_ENDOFMOVEMSGS 0x046B
#define MGMSG_NT_GET_TNA_DISPSETTINGS 0x07EA
#define MGMSG_NT_GET_TNAIOSETTINGS 0x07ED
#define MGMSG_NT_REQ_TNA_DISPSETTINGS 0x07E9
#define MGMSG_NT_REQ_TNAIOSETTINGS 0x07EC
#define MGMSG_NT_SET_EEPROMPARAMS 0x07E7
#define MGMSG_NT_SET_TNA_DISPSETTINGS 0x07E8
#define MGMSG_NT_SET_TNAIOSETTINGS 0x07EB
#define MGMSG_PZ_ACK_NTSTATUSUPDATE 0x0666
#define MGMSG_PZ_ACK_PZSTATUSUPDATE 0x0662
#define MGMSG_PZ_GET_INPUTVOLTSSRC 0x0654
#define MGMSG_PZ_GET_IOSETTINGS 0x0672
#define MGMSG_PZ_GET_MAXTRAVEL 0x0651
#define MGMSG_PZ_GET_NTCIRCCENTREPOS 0x0614
#define MGMSG_PZ_GET_NTCIRCDIALUT 0x0623
#define MGMSG_PZ_GET_NTCIRCHOMEPOS 0x0611
#define MGMSG_PZ_GET_NTCIRCPARAMS 0x0620
#define MGMSG_PZ_GET_NTFEEDBACKSRC 0x063D
#define MGMSG_PZ_GET_NTGAINPARAMS 0x0635
#define MGMSG_PZ_GET_NTMODE 0x0605
#define MGMSG_PZ_GET_NTPHASECOMPPARAMS 0x0628
#define MGMSG_PZ_GET_NTSTATUSBITS 0x063F
#define MGMSG_PZ_GET_NTSTATUSUPDATE 0x0665
#define MGMSG_PZ_GET_NTTIALPFILTERPARAMS 0x0638
#define MGMSG_PZ_GET_NTTIARANGEPARAMS 0x0632
#define MGMSG_PZ_GET_NTTIAREADING 0x063A
#define MGMSG_PZ_GET_NTTRACKTHRESHOLD 0x0608
#define MGMSG_PZ_GET_OUTPUTLUT 0x0702
#define MGMSG_PZ_GET_OUTPUTLUTPARAMS 0x0705
#define MGMSG_PZ_GET_OUTPUTMAXVOLTS 0x0682
#define MGMSG_PZ_GET_OUTPUTPOS 0x0648
#define MGMSG_PZ_GET_OUTPUTVOLTS 0x0645
#define MGMSG_PZ_GET_PICONSTS 0x0657
#define MGMSG_PZ_GET_POSCONTROLMODE 0x0642
#define MGMSG_PZ_GET_PZSTATUSBITS 0x065C
#define MGMSG_PZ_GET_PZSTATUSUPDATE 0x0661
#define MGMSG_PZ_GET_TPZ_DISPSETTINGS 0x07D3
#define MGMSG_PZ_GET_TPZ_IOSETTINGS 0x07D6
#define MGMSG_PZ_GET_TPZ_SLEWRATES 0x0685
#define MGMSG_PZ_GET_TSG_IOSETTINGS 0x07DC
#define MGMSG_PZ_GET_TSG_READING 0x07DE
#define MGMSG_PZ_MOVE_NTCIRCTOHOMEPOS 0x0612
#define MGMSG_PZ_REQ_INPUTVOLTSSRC 0x0653
#define MGMSG_PZ_REQ_IOSETTINGS 0x0671
#define MGMSG_PZ_REQ_MAXTRAVEL 0x0650
#define MGMSG_PZ_REQ_NTCIRCCENTREPOS 0x0613
#define MGMSG_PZ_REQ_NTCIRCDIALUT 0x0622
#define MGMSG_PZ_REQ_NTCIRCHOMEPOS 0x0610
#define MGMSG_PZ_REQ_NTCIRCPARAMS 0x0619
#define MGMSG_PZ_REQ_NTFEEDBACKSRC 0x063C
#define MGMSG_PZ_REQ_NTGAINPARAMS 0x0634
#define MGMSG_PZ_REQ_NTMODE 0x0604
#define MGMSG_PZ_REQ_NTPHASECOMPPARAMS 0x0627
#define MGMSG_PZ_REQ_NTSTATUSBITS 0x063E
#define MGMSG_PZ_REQ_NTSTATUSUPDATE 0x0664
#define MGMSG_PZ_REQ_NTTIALPFILTERPARAMS 0x0637
#define MGMSG_PZ_REQ_NTTIARANGEPARAMS 0x0631
#define MGMSG_PZ_REQ_NTTIAREADING 0x0639
#define MGMSG_PZ_REQ_NTTRACKTHRESHOLD 0x0607
#define MGMSG_PZ_REQ_OUTPUTLUT 0x0701
#define MGMSG_PZ_REQ_OUTPUTLUTPARAMS 0x0704
#define MGMSG_PZ_REQ_OUTPUTMAXVOLTS 0x0681
#define MGMSG_PZ_REQ_OUTPUTPOS 0x0647
#define MGMSG_PZ_REQ_OUTPUTVOLTS 0x0644
#define MGMSG_PZ_REQ_PICONSTS 0x0656
#define MGMSG_PZ_REQ_POSCONTROLMODE 0x0641
#define MGMSG_PZ_REQ_PZSTATUSBITS 0x065B
#define MGMSG_PZ_REQ_TPZ_DISPSETTINGS 0x07D2
#define MGMSG_PZ_REQ_TPZ_IOSETTINGS 0x07D5
#define MGMSG_PZ_REQ_TPZ_SLEWRATES 0x0684
#define MGMSG_PZ_REQ_TSG_IOSETTINGS 0x07DB
#define MGMSG_PZ_REQ_TSG_READING 0x07DD
#define MGMSG_PZ_SET_EEPROMPARAMS 0x07D0
#define MGMSG_PZ_SET_INPUTVOLTSSRC 0x0652
#define MGMSG_PZ_SET_IOSETTINGS 0x0670
#define MGMSG_PZ_SET_LUTVALUETYPE 0x0708
#define MGMSG_PZ_SET_NTCIRCDIA 0x061A
#define MGMSG_PZ_SET_NTCIRCDIALUT 0x0621
#define MGMSG_PZ_SET_NTCIRCHOMEPOS 0x0609
#define MGMSG_PZ_SET_NTCIRCPARAMS 0x0618
#define MGMSG_PZ_SET_NTFEEDBACKSRC 0x063B
#define MGMSG_PZ_SET_NTGAINPARAMS 0x0633
#define MGMSG_PZ_SET_NTMODE 0x0603
#define MGMSG_PZ_SET_NTPHASECOMPPARAMS 0x0626
#define MGMSG_PZ_SET_NTTIALPFILTERPARAMS 0x0636
#define MGMSG_PZ_SET_NTTIARANGEPARAMS 0x0630
#define MGMSG_PZ_SET_NTTRACKTHRESHOLD 0x0606
#define MGMSG_PZ_SET_OUTPUTLUT 0x0700
#define MGMSG_PZ_SET_OUTPUTLUTPARAMS 0x0703
#define MGMSG_PZ_SET_OUTPUTMAXVOLTS 0x0680
#define MGMSG_PZ_SET_OUTPUTPOS 0x0646
#define MGMSG_PZ_SET_OUTPUTVOLTS 0x0643
#define MGMSG_PZ_SET_PICONSTS 0x0655
#define MGMSG_PZ_SET_POSCONTROLMODE 0x0640
#define MGMSG_PZ_SET_TPZ_DISPSETTINGS 0x07D1
#define MGMSG_PZ_SET_TPZ_IOSETTINGS 0x07D4
#define MGMSG_PZ_SET_TPZ_SLEWRATES 0x0683
#define MGMSG_PZ_SET_TSG_IOSETTINGS 0x07DA
#define MGMSG_PZ_SET_ZERO 0x0658
#define MGMSG_PZ_START_LUTOUTPUT 0x0706
#define MGMSG_PZ_STOP_LUTOUTPUT 0x0707
#define MGMSG_QUAD_ACK_STATUSUPDATE 0x0882
#define MGMSG_QUAD_GET_PARAMS 0x0872
#define MGMSG_QUAD_GET_STATUSUPDATE 0x0881
#define MGMSG_QUAD_REQ_PARAMS 0x0871
#define MGMSG_QUAD_REQ_STATUSUPDATE 0x0880
#define MGMSG_QUAD_SET_EEPROMPARAMS 0x0875
#define MGMSG_QUAD_SET_PARAMS 0x0870
#define MGMSG_RACK_GET_BAYUSED 0x0061
#define MGMSG_RACK_GET_DIGOUTPUTS 0x0230
#define MGMSG_RACK_GET_STATUSBITS 0x0227
#define MGMSG_RACK_REQ_BAYUSED 0x0060
#define MGMSG_RACK_REQ_DIGOUTPUTS 0x0229
#define MGMSG_RACK_REQ_STATUSBITS 0x0226
#define MGMSG_RACK_SET_DIGOUTPUTS 0x0228
#define MGMSG_TEC_ACK_STATUSUPDATE 0x0862
#define MGMSG_TEC_GET_PARAMS 0x0842
#define MGMSG_TEC_GET_STATUSUPDATE 0x0861
#define MGMSG_TEC_REQ_PARAMS 0x0841
#define MGMSG_TEC_REQ_STATUSUPDATE 0x0860
#define MGMSG_TEC_SET_EEPROMPARAMS 0x0850
#define MGMSG_TEC_SET_PARAMS 0x0840