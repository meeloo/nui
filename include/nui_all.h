/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/


/*!
\file  nui_all.h
\brief All nui includes in one header file.

This file includes all nui headers to ease the creation of precompiled headers outside of the ngl project in VC. Just include it in your
precompiled header central file and everything should be rolling. (lookup precompiled headers in the VC doc to learn how to use them).
*/

#ifndef __nui_all_h__
#define __nui_all_h__

//#define NUI_USE_FLOATS
//#define NUI_USE_DOUBLES

#if !defined NUI_USE_DOUBLES && !defined NUI_USE_FLOATS
  #define NUI_USE_FLOATS
#endif

#ifdef NUI_USE_FLOATS
typedef float nuiSize;
typedef double nuiAltSize;
#else
typedef float nuiAltSize;
typedef double nuiSize;
#endif


#include "nuiUnicode.h"
#include "nuiToken.h"
#include "nuiObject.h"
#include "nuiTimer.h"
#include "nuiXML.h"
#include "nuiTreeEvent.h"
#include "nuiTree.h"
#include "nuiRange.h"
#include "nuiTypeTraits.h"
#include "nuiBorder.h"
#include "nuiObject.h"
#include "nuiInit.h"

#include "nuiRect.h"
#include "nuiColor.h"
#include "nglMath.h"
#include "nuiSingleton.h"

#include "nuiSampleInfo.h"
#include "nuiSampleReader.h"
#include "nuiChunkSampleReader.h"
#include "nuiSampleWriter.h"
#include "nuiWaveReader.h"
#include "nuiAiffReader.h"
#include "nuiAudioDecoder.h"
#include "nuiWaveWriter.h"
#include "nuiSample.h"
#include "nuiAudioConvert.h"
#include "nuiAiffWriter.h"


#ifndef _MINUI3_
#include "nuiPoint.h"
#include "nuiPath.h"
#include "nuiPathGenerator.h"
#include "nuiSpline.h"
#include "nuiArc.h"
#include "nuiRectPath.h"
#include "nuiPolyLine.h"
#include "nuiContour.h"
#include "nuiOutliner.h"
#include "nuiRenderArray.h"
#include "nuiPathOptimizer.h"
#include "nuiPolyLine.h"
#include "nuiTessellator.h"
#include "nuiSpline.h"
#include "nuiShape.h"
#include "nuiPoint.h"
#include "nuiDecorationDefines.h"
#include "nuiUniformDesc.h"
#include "nuiVertexAttribDesc.h"
#include "nuiRenderState.h"
#include "nuiShader.h"
#endif

#include "nuiAttributeType.h"
#include "nuiVariant.h"
#include "nuiAttribute.h"

#include "nuiAnimation.h"
#include "nuiTranslator.h"
#include "nuiSampleDefines.h"
#include "nuiMidi.h"
#include "nuiAudioDevice.h"
#include "nuiAudioDb.h"
#include "nuiSound.h"
#include "nuiVoice.h"
#include "nuiAudioEngine.h"
#include "nuiAudioResampler.h"
#include "nuiAudioFifo.h"
#include "nuiSoundManager.h"
#include "nuiMemorySound.h"
#include "nuiMemoryVoice.h"
#include "nuiSynthSound.h"
#include "nuiSynthVoice.h"
#include "nuiFileSound.h"
#include "nuiFileVoice.h"

#include "nuiStringTemplate.h"

#include "nuiCommand.h"
#include "nuiCommandContainer.h"
#include "nuiCommandManager.h"


#include "nuiTask.h"
#include "nuiTaskThread.h"
#include "nuiAttributeAnimation.h"
#include "nuiLocale.h"
#include "nuiMessageQueue.h"
#include "nuiNotification.h"

#include "nuiMemoryPool.h"

#include "nuiSerializeContext.h"
#include "nuiArchive.h"

#include "nuiRegExp.h"
#include "nuiURL.h"
#include "nuiNetworkHost.h"
#include "nuiSocket.h"
#include "nuiTCPClient.h"
#include "nuiTCPServer.h"
#include "nuiMimeMultiPart.h"
#include "nuiHTTP.h"
#include "nuiHTTPServer.h"

#include "nuiMessageData.h"
#include "nuiMessage.h"
#include "nuiMessageParser.h"
#include "nuiMessageClient.h"
#include "nuiProtocol.h"
#include "nuiDebugServer.h"

#include "nuiStopWatch.h"

#ifndef _MINUI3_
  #include "nuiApplication.h"
  #include "nuiNavigationBarStyle.h"
  #include "nuiWidgetElements.h"

  #include "nuiTextStyle.h"
  #include "nuiTextRun.h"
  #include "nuiTextLine.h"
  #include "nuiTextLayout.h"
  #include "nuiPanose.h"
  #include "nuiFontBase.h"
  #include "nuiFont.h"
  #include "nuiFontManager.h"

  #include "nuiGradient.h"
  #include "nuiDrawContext.h"
  #include "nuiSurface.h"
  #include "nuiPainter.h"
  #include "nuiGLPainter.h"
  #include "nuiMetalPainter.h"
  #include "nuiMetaPainter.h"
  #include "nuiSoftwarePainter.h"




  #include "nuiMatrixNode.h"
  #include "nuiHotKey.h"
  #include "nuiDefaultDecoration.h"
  #include "nuiMouseEvent.h"
  #include "nuiNode.h"
  #include "nuiLayer.h"
  #include "nuiWidget.h"
  #include "nuiTopLevel.h"
  #include "nuiGestureRecognizer.h"
  #include "nuiLayout.h"
  #include "nuiImageAnimation.h"
  #include "nuiImageSequence.h"
  #include "nuiWidgetMatcher.h"
  #include "nuiCSS.h"

  #include "nuiDecoration.h"
  #include "nuiBorderDecoration.h"
  #include "nuiColorDecoration.h"
  #include "nuiGradientDecoration.h"
  #include "nuiMetaDecoration.h"
  #include "nuiImageDecoration.h"
  #include "nuiStateDecoration.h"
  #include "nuiTreeHandleDecoration.h"
  #include "nuiFrame.h"


  #include "nuiGrid.h"
  #include "nuiBox.h"
  #include "nuiHBox.h"
  #include "nuiVBox.h"
  #include "nuiBuilder.h"
  #include "nuiTreeView.h"

  #include "nuiSpriteView.h"

  #include "nuiButton.h"
  #include "nuiToggleButton.h"
  #include "nuiRadioButton.h"
  #include "nuiRadioButtonGroup.h"

  #include "nuiComboBox.h"
  #include "nuiEditText.h"
  #include "nuiEditLine.h"
  #include "nuiList.h"
  #include "nuiFileList.h"
  #include "nuiFileSelector.h"
  #include "nuiFixed.h"
  #include "nuiImage.h"
  #include "nuiHugeImage.h"
  #include "nuiLabel.h"
  #include "nuiMainWindow.h"
  #include "nuiMainMenu.h"
  #include "nuiScrollBar.h"
  #include "nuiPopupMenu.h"
  #include "nuiBorder.h"
  #include "nuiSlider.h"
  #include "nuiSplitter.h"
  #include "nuiText.h"
  #include "nuiTexture.h"
  #include "nuiUserArea.h"
  #include "nuiPane.h"


  #include "nuiImageDropZone.h"
  #include "nuiShapeView.h"


  #include "nuiTabView.h"
  #include "nuiKnob.h"
  #include "nuiScrollView.h"
  #include "nuiFileTree.h"
  #include "nuiZoomView.h"
  #include "nuiScrollBox.h"
  #include "nuiTextureHelpers.h"
  #include "nuiWidgetAnimation.h"
  #include "nuiApplication.h"
  #include "nuiZoomBar.h"
  #include "nuiColorSelector.h"
  #include "nuiDrawerView.h"

  #include "nuiNavigationBar.h"
  #include "nuiNavigationController.h"
  #include "nuiNavigationButton.h"
  #include "nuiViewController.h"
  #include "nuiAttributeEditor.h"

  #include "nuiPercentAttributeEditor.h"
  #include "nuiPointAttributeEditor.h"
  #include "nuiColorAttributeEditor.h"
  #include "nuiRectAttributeEditor.h"
  #include "nuiStringAttributeEditor.h"
  #include "nuiBooleanAttributeEditor.h"
  #include "nuiSizeAttributeEditor.h"
  #include "nuiBorderAttributeEditor.h"
  #include "nuiRangeAttributeEditor.h"
  #include "nuiRangeKnobAttributeEditor.h"
  #include "nuiGenericAttributeEditor.h"
  #include "nuiPopupValueAttributeEditor.h"
  #include "nuiClampedValueAttributeEditor.h"
  #include "nuiComboAttributeEditor.h"
  #include "nuiPathAttributeEditor.h"
  #include "nuiModalContainer.h"
  #include "nuiShadeContainer.h"
  #include "nuiLabelRenamer.h"

  #include "nuiPopupView.h"

  #include "nuiBindingManager.h"

  #include "nuiTabBar.h"
  #include "nuiColumnTreeView.h"
  #include "nuiLabelAttribute.h"
  #include "nuiTitledPane.h"
  #include "nuiFolderPane.h"
  #include "nuiProgressBar.h"
  #include "nuiFlowView.h"
  #include "nuiCoverFlow.h"
  #include "nuiFontBrowser.h"
  #include "nuiFrameView.h"
  #include "nuiHotKeyEditor.h"
  #include "nuiRSS.h"
  #include "nuiRSSView.h"
  #include "nuiHyperLink.h"
  #include "nuiHTML.h"
  #include "nuiHTMLView.h"


  #include "nuiScriptEngine.h"
  #include "nuiSpiderMonkey.h"

  #include "nuiDialog.h"
  #include "nuiMessageBox.h"
  #include "nuiDialogSelectDirectory.h"
  #include "nuiDialogSelectFile.h"


  #include "nuiIntrospector.h"
  #include "nuiWidgetInspector.h"
  #include "nuiThreadInspector.h"
  #include "nuiFontInspector.h"
  #include "nuiDecorationInspector.h"
  #include "nuiTextureInspector.h"
  #include "nuiObjectInspector.h"
  #include "nuiLayersInspector.h"

  #include "nuiMidi.h"
  #include "nuiVideoDecoder.h"
  #include "nuiAVIwriter.h"

  #include "nuiRenderThread.h"

#endif


#endif // __nui_all_h__
