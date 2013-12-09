#include "MyModel.hpp"
#include <QMessageBox>

using namespace Object;

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& lScene)
{
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    pManager = FbxManager::Create();
    if( !pManager )
    {
        QMessageBox::warning(0, "Error", "Error: Unable to create FBX Manager!\n");
        exit(1);
    }

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    lScene = FbxScene::Create(pManager, "My Scene");
        if( !lScene )
    {
        QMessageBox::warning(0, "Error", "Error: Unable to create FBX scene!\n");
        exit(1);
    }
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    if( pManager ) pManager->Destroy();
        if( pExitStatus ) QMessageBox::warning(0, "Error", "Program Success!\n");
}

bool SaveScene(FbxManager* pManager, FbxDocument* lScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pManager, "");

    if( pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
        {
            if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                FbxString lDesc =pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                const char *lASCII = "ascii";
                if (lDesc.Find(lASCII)>=0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        }
    }

    // Set the export states. By default, the export states are always set to
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below
    // shows how to change these states.
    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
    {
        QMessageBox::warning(0, "Error", "FbxExporter::Initialize() failed: " + std::string(lExporter->GetLastErrorString()));
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(lScene);

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

bool LoadScene(FbxManager* pManager, FbxDocument* lScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    //char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        QMessageBox::warning(0, "Error", "Call to FbxImporter::Initialize() failed: " + std::string(lImporter->GetLastErrorString()));

        if (lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedYet ||
            lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedAnymore)
        {
            QMessageBox::warning(0, "Error", std::string("FBX file format version for file ") + pFilename + std::string("is wrong"));
        }

        return false;
    }


    if (lImporter->IsFBX())
    {

        QMessageBox::information(0, "Info", "Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        for(i = 0; i < lAnimStackCount; i++)
        {
            //FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            // Change the value of the import name if the animation stack should be imported
            // under a different name.
            //QMessageBox::warning(0, "Error", "         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported.
            //QMessageBox::warning(0, "Error", "         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            //QMessageBox::warning(0, "Error", "\n");
        }

        // Set the import states. By default, the import states are always set to
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(lScene);

    /*if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
    {
        //QMessageBox::warning(0, "Error", "Please enter password: ");
        cout << "Please enter a password: ";
        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
        scanf("%s", lPassword);
        FBXSDK_CRT_SECURE_NO_WARNING_END

        FbxString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(lScene);

        if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
        {
            QMessageBox::warning(0, "Error", "\nPassword is wrong, import aborted.\n");
        }
    }*/

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}


MyModel::MyModel(std::string file, QVector3D pos, Rotation rot) : myGLWidget(pos, rot)
{
  lSdkManager = 0;
  lScene = 0;

  // Prepare the FBX SDK.
  InitializeSdkObjects(lSdkManager, lScene);

  // Create the scene.
  int i;
  FbxTime lTime;
  FbxAnimCurveKey key;
  FbxAnimCurve* lCurve = NULL;

  // Create one animation stack
  FbxAnimStack* lAnimStack = FbxAnimStack::Create(lScene, "Stack001");

  // this stack animation range is limited from 0 to 1 second
  lAnimStack->LocalStop = FBXSDK_TIME_ONE_SECOND;
  lAnimStack->Description = "This is the animation stack description field.";

  // all animation stacks need, at least, one layer.
  FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(lScene, "Base Layer");  // the AnimLayer object name is "Base Layer"
  lAnimStack->AddMember(lAnimLayer);                                          // add the layer to the stack

  // Set and get the blend mode bypass of the layer
  bool val;
  lAnimLayer->SetBlendModeBypass(eFbxTypeCount, true);       // set the bypass to all the datatypes.
  val = lAnimLayer->GetBlendModeBypass(eFbxBool);           // val = true
  lAnimLayer->SetBlendModeBypass(eFbxBool, false);          // overwrite just for the bool datatype.
  val = lAnimLayer->GetBlendModeBypass(eFbxBool);           // val = false
  val = lAnimLayer->GetBlendModeBypass(eFbxChar);           // val = true
  val = lAnimLayer->GetBlendModeBypass(eFbxDateTime);        // val = true
  val = lAnimLayer->GetBlendModeBypass((EFbxType)-1);     // invalid type, val = false
  val = lAnimLayer->GetBlendModeBypass((EFbxType)120);    // invalid type (>MAX_TYPES), val = false


  // we want to animate the layer's weight property.
  FbxAnimCurveNode* wcn = lAnimLayer->CreateCurveNode(lAnimLayer->Weight);

  // get the Weight curve (and create it if it does not exist, wich is the case!)
  lCurve = lAnimLayer->Weight.GetCurve(lAnimLayer, "lol", "Z", true);
  if (lCurve)
    {
      // add two keys at time 0 sec and 1 sec with values 0 and 100 respectively.
      for (i = 0; i < 2; i++)
        {
          lTime.SetSecondDouble((float)i);
          key.Set(lTime, i*100.0f);
          lCurve->KeyAdd(lTime, key);
        }
    }

  //
  // now create a 3 components curvenode and animate two of the three channels.
  //
  // first, we need a "dummy" property so we can call the CreateTypedCurveNode
  FbxProperty p = FbxProperty::Create(lScene, FbxDouble3DT, "Vector3Property");
  p.Set(FbxDouble3(1.1, 2.2, 3.3));
  FbxAnimCurveNode* lCurveNode = FbxAnimCurveNode::CreateTypedCurveNode(p, lScene);

  // let's make sure the curveNode is added to the animation layer.
  lAnimLayer->AddMember(lCurveNode);

  // and to the "Vector3Property" since CreateTypedCurveNode does not make any connection
  p.ConnectSrcObject(lCurveNode);

  //Example of channel get value:
  //double v1 = lCurveNode->GetChannelValue<double>(0U, 0.0);   // v1 = 1.1
  //float  v2 = lCurveNode->GetChannelValue<float> (1U, 0.0f);  // v2 = 2.2
  //int    v3 = lCurveNode->GetChannelValue<int>   (2U, 0);     // v3 = 3

  //
  // create two free curves (not connected to anything)
  //

  // first curve
  lCurve = FbxAnimCurve::Create(lScene, "curve1");
  if (lCurve)
    {
      // add two keys at time 0 sec and 1 sec with values 0 and 10 respectively.
      for (i = 0; i < 2; i++)
        {
          lTime.SetSecondDouble((float)i);
          key.Set(lTime, i*10.0f);
          lCurve->KeyAdd(lTime, key);
        }
    }

  // connect it to the second channel
  lCurveNode->ConnectToChannel(lCurve, 1);

  // second curve
  lCurve = FbxAnimCurve::Create(lScene, "curve2");
  if (lCurve)
    {
      // add three keys
      for (i = 1; i < 4; i++)
        {
          lTime.SetSecondDouble((float)i);
          key.Set(lTime, i*3.33f);
          lCurve->KeyAdd(lTime, key);
        }
    }
  // connect it to the third channel
  lCurveNode->ConnectToChannel(lCurve, "Z"); // for backward compatibility, string identifier are still
  // allowed for the X,Y,Z and W components or "0", "1", ... "9", "A", "B", ... "F" for the Matrix44 datatype


  // ======================================================================
  //
  // Add a second animation layer and evaluate using the FbxAnimEvaluator
  //
  // ======================================================================
  lAnimLayer = FbxAnimLayer::Create(lScene, "Layer2");
  lAnimStack->AddMember(lAnimLayer);

  // get the number of animation layers in the stack
  //int nbLayers = lAnimStack->GetMemberCount(FBX_TYPE(FbxAnimLayer));  // nblayers = 2
  lAnimLayer = lAnimStack->GetMember(FBX_TYPE(FbxAnimLayer), 1);      // get the second layer

  // set its blend mode to Additive
  lAnimLayer->BlendMode.Set(FbxAnimLayer::eBlendAdditive);

  // Now, let's animate the firrt channel of the "Vector3Property" (remember, we animated the second and
  // third on the base layer)
  // but firs, make sure the property is animatable otherwise the creation of the curveNode is prohibited.
  p.ModifyFlag(FbxPropertyAttr::eAnimatable, true);
  lCurveNode = p.GetCurveNode(lAnimLayer, true); // create it since it does not exist yet

  // use "curve2" to animate it
  lCurveNode->ConnectToChannel(lCurve, 0U);

  // and set the other two channels values
  lCurveNode->SetChannelValue<double>(1U, 5.0);
  lCurveNode->SetChannelValue<double>(2U, 0.0);

  // evaluate the "Vector3Property" value at three different times
  // with the use of the FbxAnimEvaluator so we take into account the two layers

  // make sure the evaluator is using the correct context (context == animstack)
  lScene->GetEvaluator()->SetContext(lAnimStack);
  for (i = 0; i < 3; i++)
    {
      lTime.SetSecondDouble((float)i*0.33f);
      FbxAnimCurveNode& val = lScene->GetEvaluator()->GetPropertyValue(p, lTime);

      double v[3];
      v[0] = val.GetChannelValue<double>(0U, 0.0);
      v[1] = val.GetChannelValue<double>(1U, 0.0);
      v[2] = val.GetChannelValue<double>(2U, 0.0);
    }
}

MyModel::~MyModel()
{
  lSdkManager->Destroy();
}

/*


Main Page
Classes
Files
Directories
Examples
Animation/main.cxx

/****************************************************************************************

   Copyright (C) 2012 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************

//
// Illustrates the use of animation stacks, layers, curvenodes and curves.
//

#include <fbxsdk.h>
#include "../Common/Common.h"

// Function prototypes.
bool CreateScene(FbxManager* pSdkManager, FbxScene* lScene);

int main(int /*argc, char** argv)
{
    FbxManager* lSdkManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;

    // Prepare the FBX SDK.
    InitializeSdkObjects(lSdkManager, lScene);

    // Create the scene.
    lResult = CreateScene(lSdkManager, lScene);
    if(lResult == false)
    {
        QMessageBox::warning(0, "Error", "\n\nAn error occurred while creating the scene...\n");
        DestroySdkObjects(lSdkManager);
        return 0;
    }

    // Destroy all objects created by the FBX SDK.
    DestroySdkObjects(lSdkManager);

    return 0;
}

bool CreateScene(FbxManager* /*pSdkManager*//*, FbxScene* lScene)
{
    int i;
    FbxTime lTime;
    FbxAnimCurveKey key;
    FbxAnimCurve* lCurve = NULL;

    // Create one animation stack
    FbxAnimStack* lAnimStack = FbxAnimStack::Create(lScene, "Stack001");

    // this stack animation range is limited from 0 to 1 second
    lAnimStack->LocalStop = FBXSDK_TIME_ONE_SECOND;
    lAnimStack->Description = "This is the animation stack description field.";

    // all animation stacks need, at least, one layer.
    FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(lScene, "Base Layer");  // the AnimLayer object name is "Base Layer"
    lAnimStack->AddMember(lAnimLayer);                                          // add the layer to the stack

    // Set and get the blend mode bypass of the layer
    bool val;
    lAnimLayer->SetBlendModeBypass(eFbxTypeCount, true);       // set the bypass to all the datatypes.
    val = lAnimLayer->GetBlendModeBypass(eFbxBool);           // val = true
    lAnimLayer->SetBlendModeBypass(eFbxBool, false);          // overwrite just for the bool datatype.
    val = lAnimLayer->GetBlendModeBypass(eFbxBool);           // val = false
    val = lAnimLayer->GetBlendModeBypass(eFbxChar);           // val = true
    val = lAnimLayer->GetBlendModeBypass(eFbxDateTime);        // val = true
    val = lAnimLayer->GetBlendModeBypass((EFbxType)-1);     // invalid type, val = false
    val = lAnimLayer->GetBlendModeBypass((EFbxType)120);    // invalid type (>MAX_TYPES), val = false


    // we want to animate the layer's weight property.
    FbxAnimCurveNode* wcn = lAnimLayer->CreateCurveNode(lAnimLayer->Weight);
    if (wcn)
    {
        // the curve node from the Weight property already contains 1 channel (Weight).
        i = wcn->GetChannelsCount();                            // i = 1

        // Now, let's add a second channel to the animation node. Note that this code
        // is useless and has only been provided to show the usage of the AddChannel and
        // ResetChannels
        bool ret;
        ret = wcn->AddChannel<int>("MyAddedIntChannel", 99);    // this call will succed
        i = wcn->GetChannelsCount();                            // i = 2
        ret = wcn->AddChannel<int>("MyAddedIntChannel", 10);    // this call will fail, since the channel already exists.
        i = wcn->GetChannelsCount();                            // i = 2
        wcn->ResetChannels();                                   // remove any added channels
        i = wcn->GetChannelsCount();                            // i = 1
    }

    // get the Weight curve (and create it if it does not exist, wich is the case!)
    lCurve = lAnimLayer->Weight.GetCurve<FbxAnimCurve>(lAnimLayer, true);
    if (lCurve)
    {
        // add two keys at time 0 sec and 1 sec with values 0 and 100 respectively.
        for (i = 0; i < 2; i++)
        {
            lTime.SetSecondDouble((float)i);
            key.Set(lTime, i*100.0f);
            lCurve->KeyAdd(lTime, key);
        }
    }

    //
    // now create a 3 components curvenode and animate two of the three channels.
    //
    // first, we need a "dummy" property so we can call the CreateTypedCurveNode
    FbxProperty p = FbxProperty::Create(lScene, FbxDouble3DT, "Vector3Property");
    p.Set(FbxDouble3(1.1, 2.2, 3.3));
    FbxAnimCurveNode* lCurveNode = FbxAnimCurveNode::CreateTypedCurveNode(p, lScene);

    // let's make sure the curveNode is added to the animation layer.
    lAnimLayer->AddMember(lCurveNode);

    // and to the "Vector3Property" since CreateTypedCurveNode does not make any connection
    p.ConnectSrcObject(lCurveNode);

    //Example of channel get value:
    //double v1 = lCurveNode->GetChannelValue<double>(0U, 0.0);   // v1 = 1.1
    //float  v2 = lCurveNode->GetChannelValue<float> (1U, 0.0f);  // v2 = 2.2
    //int    v3 = lCurveNode->GetChannelValue<int>   (2U, 0);     // v3 = 3

    //
    // create two free curves (not connected to anything)
    //

    // first curve
    lCurve = FbxAnimCurve::Create(lScene, "curve1");
    if (lCurve)
    {
        // add two keys at time 0 sec and 1 sec with values 0 and 10 respectively.
        for (i = 0; i < 2; i++)
        {
            lTime.SetSecondDouble((float)i);
            key.Set(lTime, i*10.0f);
            lCurve->KeyAdd(lTime, key);
        }
    }

    // connect it to the second channel
    lCurveNode->ConnectToChannel(lCurve, 1);

    // second curve
    lCurve = FbxAnimCurve::Create(lScene, "curve2");
    if (lCurve)
    {
        // add three keys
        for (i = 1; i < 4; i++)
        {
            lTime.SetSecondDouble((float)i);
            key.Set(lTime, i*3.33f);
            lCurve->KeyAdd(lTime, key);
        }
    }
    // connect it to the third channel
    lCurveNode->ConnectToChannel(lCurve, "Z"); // for backward compatibility, string identifier are still
    // allowed for the X,Y,Z and W components or "0", "1", ... "9", "A", "B", ... "F" for the Matrix44 datatype


    // ======================================================================
    //
    // Add a second animation layer and evaluate using the FbxAnimEvaluator
    //
    // ======================================================================
    lAnimLayer = FbxAnimLayer::Create(lScene, "Layer2");
    lAnimStack->AddMember(lAnimLayer);

    // get the number of animation layers in the stack
    //int nbLayers = lAnimStack->GetMemberCount(FBX_TYPE(FbxAnimLayer));  // nblayers = 2
    lAnimLayer = lAnimStack->GetMember(FBX_TYPE(FbxAnimLayer), 1);      // get the second layer

    // set its blend mode to Additive
    lAnimLayer->BlendMode.Set(FbxAnimLayer::eBlendAdditive);

    // Now, let's animate the firrt channel of the "Vector3Property" (remember, we animated the second and
    // third on the base layer)
    // but firs, make sure the property is animatable otherwise the creation of the curveNode is prohibited.
    p.ModifyFlag(FbxPropertyAttr::eAnimatable, true);
    lCurveNode = p.GetCurveNode(lAnimLayer, true); // create it since it does not exist yet

    // use "curve2" to animate it
    lCurveNode->ConnectToChannel(lCurve, 0U);

    // and set the other two channels values
    lCurveNode->SetChannelValue<double>(1U, 5.0);
    lCurveNode->SetChannelValue<double>(2U, 0.0);

    // evaluate the "Vector3Property" value at three different times
    // with the use of the FbxAnimEvaluator so we take into account the two layers

    // make sure the evaluator is using the correct context (context == animstack)
    lScene->GetEvaluator()->SetContext(lAnimStack);
    for (i = 0; i < 3; i++)
    {
        lTime.SetSecondDouble((float)i*0.33f);
        FbxAnimCurveNode& val = lScene->GetEvaluator()->GetPropertyValue(p, lTime);

        double v[3];
        v[0] = val.GetChannelValue<double>(0U, 0.0);
        v[1] = val.GetChannelValue<double>(1U, 0.0);
        v[2] = val.GetChannelValue<double>(2U, 0.0);
    }

    /* the evaluated values for v at 0, .333 and .999 seconds are:

          time  |    0.0      |      0.33      |    0.99      |
       val      +-------------+----------------+--------------|
            0   |  3.33       |     3.69       |    4.05      |
            1   |  5.0        |     6.08       |    9.35      |
            2   |  0.0        |     1.098      |    2.19      |
                +-------------+----------------+--------------|
    *//*
    return true;
}

Generated on Mon Jul 30 2012 18:02:55 by  1.7.3
Please send us your comment about this page
  */
