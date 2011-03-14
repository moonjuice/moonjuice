; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=Base_Dalog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "vision1.h"
LastPage=0

ClassCount=7
Class1=CVision1App
Class2=CVision1Doc
Class3=CVision1View
Class4=CMainFrame

ResourceCount=10
Resource1=IDD_BASE
Resource2=IDD_MASk (Chinese (Taiwan))
Class5=CAboutDlg
Resource3=IDD_ABOUTBOX
Class6=Base_Dalog
Resource4=IDR_MAINFRAME
Class7=Mask
Resource5=IDD_VISION1_FORM
Resource6=IDD_VISION1_FORM (Chinese (Taiwan))
Resource7=IDD_BASE (Chinese (Taiwan))
Resource8=IDD_ABOUTBOX (Chinese (Taiwan))
Resource9=IDR_MAINFRAME (Chinese (Taiwan))
Resource10=IDD_MASk

[CLS:CVision1App]
Type=0
HeaderFile=vision1.h
ImplementationFile=vision1.cpp
Filter=N

[CLS:CVision1Doc]
Type=0
HeaderFile=vision1Doc.h
ImplementationFile=vision1Doc.cpp
Filter=N

[CLS:CVision1View]
Type=0
HeaderFile=vision1View.h
ImplementationFile=vision1View.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CVision1View


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T




[CLS:CAboutDlg]
Type=0
HeaderFile=vision1.cpp
ImplementationFile=vision1.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_APP_ABOUT
CommandCount=16

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_VISION1_FORM]
Type=1
Class=CVision1View
ControlCount=3
Control1=IDC_BUTTON1,button,1342263040
Control2=IDC_STATIC,static,1350569998
Control3=IDC_STATIC,static,1342177294

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[DLG:IDD_BASE]
Type=1
Class=Base_Dalog
ControlCount=51
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LOAD_IMAGE,button,1342242816
Control4=IDC_Picture1,static,1342177295
Control5=IDC_Picture2,static,1342177295
Control6=IDC_SLIDER1,msctls_trackbar32,1342242821
Control7=IDC_Threshlod,edit,1350631552
Control8=IDC_Two_Value,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SAVE_IMAGE,button,1342242816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,button,1342178055
Control14=IDC_HISTOGRAM,button,1342242816
Control15=IDC_RETURN,button,1342242816
Control16=IDC_STATIC,button,1342178055
Control17=IDC_Laplacian,button,1342242816
Control18=IDC_COMBO1,combobox,1344340227
Control19=IDC_STATIC,static,1342308352
Control20=IDC_Sobel,button,1342242816
Control21=IDC_COMBO2,combobox,1344340227
Control22=IDC_STATIC,static,1342308352
Control23=IDC_Return_num,edit,1350631552
Control24=IDC_STATIC,button,1342178055
Control25=IDC_HightPass,button,1342242816
Control26=IDC_COMBO3,combobox,1344340227
Control27=IDC_LowPass,button,1342242816
Control28=IDC_COMBO4,combobox,1344340227
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_Delete_Noise,button,1342242816
Control32=IDC_BW_Change,button,1342242816
Control33=IDC_Image_Smoth,button,1342242816
Control34=IDC_Contrast,button,1342242816
Control35=IDC_STATIC,static,1342308352
Control36=IDC_HISTOGRAM_Sliding,button,1342242816
Control37=IDC_HISTOGRAM_Stretch,button,1342242816
Control38=IDC_STATIC,button,1342178055
Control39=IDC_H_S_Value,edit,1350631552
Control40=IDC_H_STR_Value,edit,1350631552
Control41=IDC_STATIC,static,1342308352
Control42=IDC_STATIC,static,1342308352
Control43=IDC_Disp_Edge,button,1342242816
Control44=IDC_Line_Edge,button,1342242816
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1342308352
Control47=IDC_COMBO7,combobox,1344340227
Control48=IDC_COMBO8,combobox,1344340227
Control49=IDC_DEL_Pixel,edit,1350631552
Control50=IDC_STATIC,static,1342308352
Control51=IDC_STATIC,button,1342178055

[CLS:Base_Dalog]
Type=0
HeaderFile=base_dalog.h
ImplementationFile=base_dalog.cpp
BaseClass=CDialog
LastObject=Base_Dalog
Filter=D
VirtualFilter=dWC

[DLG:IDD_MASk]
Type=1
Class=Mask
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_EDIT2,edit,1350631552
Control5=IDC_EDIT3,edit,1350631552
Control6=IDC_EDIT4,edit,1350631552
Control7=IDC_EDIT5,edit,1350631552
Control8=IDC_EDIT6,edit,1350631552
Control9=IDC_EDIT7,edit,1350631552
Control10=IDC_EDIT8,edit,1350631552
Control11=IDC_EDIT9,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352

[CLS:Mask]
Type=0
HeaderFile=Mask.h
ImplementationFile=Mask.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT1

[DLG:IDD_ABOUTBOX (Chinese (Taiwan))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_VISION1_FORM (Chinese (Taiwan))]
Type=1
Class=?
ControlCount=3
Control1=IDC_BUTTON1,button,1342263040
Control2=IDC_STATIC,static,1350569998
Control3=IDC_STATIC,static,1342177294

[DLG:IDD_MASk (Chinese (Taiwan))]
Type=1
Class=?
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_EDIT2,edit,1350631552
Control5=IDC_EDIT3,edit,1350631552
Control6=IDC_EDIT4,edit,1350631552
Control7=IDC_EDIT5,edit,1350631552
Control8=IDC_EDIT6,edit,1350631552
Control9=IDC_EDIT7,edit,1350631552
Control10=IDC_EDIT8,edit,1350631552
Control11=IDC_EDIT9,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352

[DLG:IDD_BASE (Chinese (Taiwan))]
Type=1
Class=?
ControlCount=51
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LOAD_IMAGE,button,1342242816
Control4=IDC_Picture1,static,1342177295
Control5=IDC_Picture2,static,1342177295
Control6=IDC_SLIDER1,msctls_trackbar32,1342242821
Control7=IDC_Threshlod,edit,1350631552
Control8=IDC_Two_Value,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_SAVE_IMAGE,button,1342242816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,button,1342178055
Control14=IDC_HISTOGRAM,button,1342242816
Control15=IDC_RETURN,button,1342242816
Control16=IDC_STATIC,button,1342178055
Control17=IDC_Laplacian,button,1342242816
Control18=IDC_COMBO1,combobox,1344340227
Control19=IDC_STATIC,static,1342308352
Control20=IDC_Sobel,button,1342242816
Control21=IDC_COMBO2,combobox,1344340227
Control22=IDC_STATIC,static,1342308352
Control23=IDC_Return_num,edit,1350631552
Control24=IDC_STATIC,button,1342178055
Control25=IDC_HightPass,button,1342242816
Control26=IDC_COMBO3,combobox,1344340227
Control27=IDC_LowPass,button,1342242816
Control28=IDC_COMBO4,combobox,1344340227
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_Delete_Noise,button,1342242816
Control32=IDC_BW_Change,button,1342242816
Control33=IDC_Image_Smoth,button,1342242816
Control34=IDC_Contrast,button,1342242816
Control35=IDC_STATIC,static,1342308352
Control36=IDC_HISTOGRAM_Sliding,button,1342242816
Control37=IDC_HISTOGRAM_Stretch,button,1342242816
Control38=IDC_STATIC,button,1342178055
Control39=IDC_H_S_Value,edit,1350631552
Control40=IDC_H_STR_Value,edit,1350631552
Control41=IDC_STATIC,static,1342308352
Control42=IDC_STATIC,static,1342308352
Control43=IDC_Disp_Edge,button,1342242816
Control44=IDC_Line_Edge,button,1342242816
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1342308352
Control47=IDC_COMBO7,combobox,1344340227
Control48=IDC_COMBO8,combobox,1344340227
Control49=IDC_DEL_Pixel,edit,1350631552
Control50=IDC_STATIC,static,1342308352
Control51=IDC_STATIC,button,1342178055

[TB:IDR_MAINFRAME (Chinese (Taiwan))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME (Chinese (Taiwan))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_APP_ABOUT
CommandCount=16

[ACL:IDR_MAINFRAME (Chinese (Taiwan))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

