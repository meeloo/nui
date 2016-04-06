/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "../graphics/DefaultDecoration.h"


nuiDefaultDecoration::nuiDefaultDecoration()
{
  NGL_ASSERT(0);
}


nuiDefaultDecoration::~nuiDefaultDecoration()
{
  NGL_ASSERT(0);
} 


//**************************************************************************************************************
//
// Init
//
// connect the object class to the decoration method
//
void nuiDefaultDecoration::Init()
{
  nuiInitDefaultDecorationAtlas();
  
  InitColors();
  InitSelection();
  InitMaps();
  InitIcons();
  InitImages();
    
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiLabel"), &nuiDefaultDecoration::Label);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiEditText"), &nuiDefaultDecoration::EditText);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiEditLine"), &nuiDefaultDecoration::EditLine);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiSplitter"), &nuiDefaultDecoration::Splitter);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiTitledPane"), &nuiDefaultDecoration::TitledPane);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFolderPane"), &nuiDefaultDecoration::FolderPane);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiButton"), &nuiDefaultDecoration::Button);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiRadioButton"), &nuiDefaultDecoration::RadioButton);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiCloseButton"), &nuiDefaultDecoration::CloseButton);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiToggleButton"), &nuiDefaultDecoration::ToggleButton);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiKnob"), &nuiDefaultDecoration::KnobSequence);

  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiComboBox"), &nuiDefaultDecoration::ComboBox);

  // dialogs
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiDialog::Title"), &nuiDefaultDecoration::Dialog_Title);
  
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiDialog::EditLine"), &nuiDefaultDecoration::Dialog_EditLine);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiMessageBox::Title"), &nuiDefaultDecoration::MessageBox_Title);
  
  
  // FileSelector
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::FolderView"), &nuiDefaultDecoration::FileSelector_FolderView);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::InfoView"), &nuiDefaultDecoration::FileSelector_InfoView);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::FolderLabel"), &nuiDefaultDecoration::FileSelector_FolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::VolumeIcon"), &nuiDefaultDecoration::FileSelector_VolumeIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::FolderIcon"), &nuiDefaultDecoration::FileSelector_FolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::TreeView"), &nuiDefaultDecoration::FileSelector_TreeView);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::TreeFolderLabel"), &nuiDefaultDecoration::FileSelector_TreeFolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::TreeFileLabel"), &nuiDefaultDecoration::FileSelector_TreeFileLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::TreeFolderIcon"), &nuiDefaultDecoration::FileSelector_TreeFolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileSelector::TreeFileIcon"), &nuiDefaultDecoration::FileSelector_TreeFileIcon);

  // FileTree
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree"), &nuiDefaultDecoration::FileTree_View);
//  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::Node"), &nuiDefaultDecoration::FileTree_Node);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::ColumnLabel"), &nuiDefaultDecoration::FileTree_ColumnLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::VolumeLabel"), &nuiDefaultDecoration::FileTree_FolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::FolderLabel"), &nuiDefaultDecoration::FileTree_FolderLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::FileLabel"), &nuiDefaultDecoration::FileTree_FileLabel);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::FileInfo"), &nuiDefaultDecoration::FileTree_FileInfo);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::VolumeIcon"), &nuiDefaultDecoration::FileTree_VolumeIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::FolderIcon"), &nuiDefaultDecoration::FileTree_FolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::OpenFolderIcon"), &nuiDefaultDecoration::FileTree_OpenFolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::ParentFolderIcon"), &nuiDefaultDecoration::FileTree_ParentFolderIcon);
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiFileTree::FileIcon"), &nuiDefaultDecoration::FileTree_FileIcon);

  // NavigationViews
  nuiWidget::SetDefaultDecoration(nuiObject::GetClassNameIndex("nuiNavigationBar"), &nuiDefaultDecoration::NavigationBar);
  
  
  App->AddExit(nuiDefaultDecoration::Exit);
}


void nuiDefaultDecoration::Exit()
{
  {
    // Release the icons:
    std::list<nuiTexture*>::iterator it = mIcons.begin();
    std::list<nuiTexture*>::iterator end = mIcons.end();
    while (it != end)
    {
      nuiTexture* pTex = *it;
      pTex->Release();
      ++it;
    }
    mIcons.clear();
  }

  mImages.clear();
  
  nuiWidget::ClearDefaultDecorations();
}



//**************************************************************************************************************
//
// InitColors
//


typedef struct static_color
{
  nglChar* name;
  uint8 r,g,b;
} static_color;

static static_color static_colors[] =
{
  // default colors for decoration
  {(nglChar*)"nuiDefaultColorPane1", 240,240,240},
  {(nglChar*)"nuiDefaultColorPane2", 252,252,252},
  {(nglChar*)"nuiDefaultColorStroke", 205,205,205},

  {(nglChar*)"nuiDefaultColorLabelText", 40,40,40},

  {(nglChar*)"nuiDefaultColorScrollBar1", 169,181,200},
  {(nglChar*)"nuiDefaultColorScrollBar2", 91,115,150},
  {(nglChar*)"nuiDefaultColorScrollBar3", 34,66,114},
  {(nglChar*)"nuiDefaultColorScrollBarStroke", 94,117,152},

  {(nglChar*)"nuiHyperLink", 64,64,255},


  // constants (web colors)
  {(nglChar*)"aliceblue", 240, 248, 255},
  {(nglChar*)"antiquewhite", 250, 235, 215},
  {(nglChar*)"aqua",  0, 255, 255},
  {(nglChar*)"aquamarine", 127, 255, 212},
  {(nglChar*)"azure", 240, 255, 255},
  {(nglChar*)"beige", 245, 245, 220},
  {(nglChar*)"bisque", 255, 228, 196},
  {(nglChar*)"black",  0, 0, 0},
  {(nglChar*)"blanchedalmond", 255, 235, 205},
  {(nglChar*)"blue",  0, 0, 255},
  {(nglChar*)"blueviolet", 138, 43, 226},
  {(nglChar*)"brown", 165, 42, 42},
  {(nglChar*)"burlywood", 222, 184, 135},
  {(nglChar*)"cadetblue",  95, 158, 160},
  {(nglChar*)"chartreuse", 127, 255, 0},
  {(nglChar*)"chocolate", 210, 105, 30},
  {(nglChar*)"coral", 255, 127, 80},
  {(nglChar*)"cornflowerblue", 100, 149, 237},
  {(nglChar*)"cornsilk", 255, 248, 220},
  {(nglChar*)"crimson", 220, 20, 60},
  {(nglChar*)"cyan",  0, 255, 255},
  {(nglChar*)"darkblue",  0, 0, 139},
  {(nglChar*)"darkcyan",  0, 139, 139},
  {(nglChar*)"darkgoldenrod", 184, 134, 11},
  {(nglChar*)"darkgray", 169, 169, 169},
  {(nglChar*)"darkgreen",  0, 100, 0},
  {(nglChar*)"darkgrey", 169, 169, 169},
  {(nglChar*)"darkkhaki", 189, 183, 107},
  {(nglChar*)"darkmagenta", 139, 0, 139},
  {(nglChar*)"darkolivegreen",  85, 107, 47},
  {(nglChar*)"darkorange", 255, 140, 0},
  {(nglChar*)"darkorchid", 153, 50, 204},
  {(nglChar*)"darkred", 139, 0, 0},
  {(nglChar*)"darksalmon", 233, 150, 122},
  {(nglChar*)"darkseagreen", 143, 188, 143},
  {(nglChar*)"darkslateblue",  72, 61, 139},
  {(nglChar*)"darkslategray",  47, 79, 79},
  {(nglChar*)"darkslategrey",  47, 79, 79},
  {(nglChar*)"darkturquoise",  0, 206, 209},
  {(nglChar*)"darkviolet", 148, 0, 211},
  {(nglChar*)"deeppink", 255, 20, 147},
  {(nglChar*)"deepskyblue",  0, 191, 255},
  {(nglChar*)"dimgray", 105, 105, 105},
  {(nglChar*)"dimgrey", 105, 105, 105},
  {(nglChar*)"dodgerblue",  30, 144, 255},
  {(nglChar*)"firebrick", 178, 34, 34},
  {(nglChar*)"floralwhite", 255, 250, 240},
  {(nglChar*)"forestgreen",  34, 139, 34},
  {(nglChar*)"fuchsia", 255, 0, 255},
  {(nglChar*)"gainsboro", 220, 220, 220},
  {(nglChar*)"ghostwhite", 248, 248, 255},
  {(nglChar*)"gold", 255, 215, 0},
  {(nglChar*)"goldenrod", 218, 165, 32},
  {(nglChar*)"gray", 128, 128, 128},
  {(nglChar*)"grey", 128, 128, 128},
  {(nglChar*)"green",  0, 128, 0},
  {(nglChar*)"greenyellow", 173, 255, 47},
  {(nglChar*)"honeydew", 240, 255, 240},
  {(nglChar*)"hotpink", 255, 105, 180},
  {(nglChar*)"indianred", 205, 92, 92},
  {(nglChar*)"indigo",  75, 0, 130},
  {(nglChar*)"ivory", 255, 255, 240},
  {(nglChar*)"khaki", 240, 230, 140},
  {(nglChar*)"lavender", 230, 230, 250},
  {(nglChar*)"lavenderblush", 255, 240, 245},
  {(nglChar*)"lawngreen", 124, 252, 0},
  {(nglChar*)"lemonchiffon", 255, 250, 205},
  {(nglChar*)"lightblue", 173, 216, 230},
  {(nglChar*)"lightcoral", 240, 128, 128},
  {(nglChar*)"lightcyan", 224, 255, 255},
  {(nglChar*)"lightgoldenrodyellow", 250, 250, 210},
  {(nglChar*)"lightgray", 211, 211, 211},
  {(nglChar*)"lightgreen", 144, 238, 144},
  {(nglChar*)"lightgrey", 211, 211, 211},
  {(nglChar*)"lightpink", 255, 182, 193},
  {(nglChar*)"lightsalmon", 255, 160, 122},
  {(nglChar*)"lightseagreen",  32, 178, 170},
  {(nglChar*)"lightskyblue", 135, 206, 250},
  {(nglChar*)"lightslategray", 119, 136, 153},
  {(nglChar*)"lightslategrey", 119, 136, 153},
  {(nglChar*)"lightsteelblue", 176, 196, 222},
  {(nglChar*)"lightyellow", 255, 255, 224},
  {(nglChar*)"lime",  0, 255, 0},
  {(nglChar*)"limegreen",  50, 205, 50},
  {(nglChar*)"linen", 250, 240, 230},
  {(nglChar*)"magenta", 255, 0, 255},
  {(nglChar*)"maroon", 128, 0, 0},
  {(nglChar*)"mediumaquamarine", 102, 205, 170},
  {(nglChar*)"mediumblue",  0, 0, 205},
  {(nglChar*)"mediumorchid", 186, 85, 211},
  {(nglChar*)"mediumpurple", 147, 112, 219},
  {(nglChar*)"mediumseagreen",  60, 179, 113},
  {(nglChar*)"mediumslateblue", 123, 104, 238},
  {(nglChar*)"mediumspringgreen",  0, 250, 154},
  {(nglChar*)"mediumturquoise",  72, 209, 204},
  {(nglChar*)"mediumvioletred", 199, 21, 133},
  {(nglChar*)"midnightblue",  25, 25, 112},
  {(nglChar*)"mintcream", 245, 255, 250},
  {(nglChar*)"mistyrose", 255, 228, 225},
  {(nglChar*)"moccasin", 255, 228, 181},
  {(nglChar*)"navajowhite", 255, 222, 173},
  {(nglChar*)"navy",  0, 0, 128},
  {(nglChar*)"oldlace", 253, 245, 230},
  {(nglChar*)"olive", 128, 128, 0},
  {(nglChar*)"olivedrab", 107, 142, 35},
  {(nglChar*)"orange", 255, 165, 0},
  {(nglChar*)"orangered", 255, 69, 0},
  {(nglChar*)"orchid", 218, 112, 214},
  {(nglChar*)"palegoldenrod", 238, 232, 170},
  {(nglChar*)"palegreen", 152, 251, 152},
  {(nglChar*)"paleturquoise", 175, 238, 238},
  {(nglChar*)"palevioletred", 219, 112, 147},
  {(nglChar*)"papayawhip", 255, 239, 213},
  {(nglChar*)"peachpuff", 255, 218, 185},
  {(nglChar*)"peru", 205, 133, 63},
  {(nglChar*)"pink", 255, 192, 203},
  {(nglChar*)"plum", 221, 160, 221},
  {(nglChar*)"powderblue", 176, 224, 230},
  {(nglChar*)"purple", 128, 0, 128},
  {(nglChar*)"red", 255, 0, 0},
  {(nglChar*)"rosybrown", 188, 143, 143},
  {(nglChar*)"royalblue",  65, 105, 225},
  {(nglChar*)"saddlebrown", 139, 69, 19},
  {(nglChar*)"salmon", 250, 128, 114},
  {(nglChar*)"sandybrown", 244, 164, 96},
  {(nglChar*)"seagreen",  46, 139, 87},
  {(nglChar*)"seashell", 255, 245, 238},
  {(nglChar*)"sienna", 160, 82, 45},
  {(nglChar*)"silver", 192, 192, 192},
  {(nglChar*)"skyblue", 135, 206, 235},
  {(nglChar*)"slateblue", 106, 90, 205},
  {(nglChar*)"slategray", 112, 128, 144},
  {(nglChar*)"slategrey", 112, 128, 144},
  {(nglChar*)"snow", 255, 250, 250},
  {(nglChar*)"springgreen",  0, 255, 127},
  {(nglChar*)"steelblue",  70, 130, 180},
  {(nglChar*)"tan", 210, 180, 140},
  {(nglChar*)"teal",  0, 128, 128},
  {(nglChar*)"thistle", 216, 191, 216},
  {(nglChar*)"tomato", 255, 99, 71},
  {(nglChar*)"turquoise",  64, 224, 208},
  {(nglChar*)"violet", 238, 130, 238},
  {(nglChar*)"wheat", 245, 222, 179},
  {(nglChar*)"white", 255, 255, 255},
  {(nglChar*)"whitesmoke", 245, 245, 245},
  {(nglChar*)"yellow", 255, 255, 0},
  {(nglChar*)"yellowgreen", 154, 205, 50},
  {(nglChar*)"flat_turquoise", 26, 188, 156},
  {(nglChar*)"flat_greensea", 22, 160, 133},
  {(nglChar*)"flat_emerald", 46, 204, 113},
  {(nglChar*)"flat_nephritis", 39, 174, 96},
  {(nglChar*)"flat_peterriver", 52, 152, 219},
  {(nglChar*)"flat_belizehole", 41, 128, 185},
  {(nglChar*)"flat_amethyst", 155, 89, 182},
  {(nglChar*)"flat_wisteria", 142, 68, 173},
  {(nglChar*)"flat_wetasphalt", 52, 73, 94},
  {(nglChar*)"flat_midnightblue", 44, 62, 80},
  {(nglChar*)"flat_sunflower", 241, 196, 15},
  {(nglChar*)"flat_orange", 243, 156, 18},
  {(nglChar*)"flat_carrot", 230, 126, 34},
  {(nglChar*)"flat_pumpkin", 211, 84, 0},
  {(nglChar*)"flat_alizarin", 231, 76, 60},
  {(nglChar*)"flat_pomegranate", 192, 57, 43},
  {(nglChar*)"flat_clouds", 236, 240, 241},
  {(nglChar*)"flat_silver", 189, 195, 199},
  {(nglChar*)"flat_concrete", 149, 165, 166},
  {(nglChar*)"flat_asbestos", 127, 140, 141},

  {NULL, 0, 0, 0}
  
};    

void nuiDefaultDecoration::InitColors()
{
  const float ratio = 1.0f / 255.0f;
  uint i;
  // try to find a static color name that fit the given color name:
  for (i = 0; static_colors[i].name; i++)
  {
    nuiColor::SetColor(static_colors[i].name, nuiColor((static_colors[i].r * ratio), (static_colors[i].g * ratio), (static_colors[i].b * ratio), 1.0));
  }


  nuiColor::SetColor("nuiDefaultClrWindowBkg1",         nuiColor(225,230,235));
  nuiColor::SetColor("nuiDefaultClrWindowBkg2",         nuiColor(255,255,255));

  nuiColor::SetColor("nuiDefaultClrCaptionBkg1",        nuiColor(197,197,197));
  nuiColor::SetColor("nuiDefaultClrCaptionBkg2",        nuiColor(150,150,150));
  
  nuiColor::SetColor("nuiDefaultClrInactiveCaptionBkg1", nuiColor(157,157,157));
  nuiColor::SetColor("nuiDefaultClrInactiveCaptionBkg2", nuiColor(110,110,110));
  

  nuiColor::SetColor("nuiDefaultClrCaptionBorder",      nuiColor(150,150,150));
  nuiColor::SetColor("nuiDefaultClrCaptionBorderLight", nuiColor(238,238,238));
  nuiColor::SetColor("nuiDefaultClrCaptionBorderDark",  nuiColor(64,64,64));
  nuiColor::SetColor("nuiDefaultClrInactiveCaptionBorderLight", nuiColor(198,198,198));
  
  nuiColor::SetColor("nuiDefaultClrCaptionText",        nuiColor(16,16,16));
  nuiColor::SetColor("nuiDefaultClrCaptionTextLight",   nuiColor(215,215,215));
  nuiColor::SetColor("nuiDefaultClrInactiveCaptionTextLight", nuiColor(175,175,175));
  
  nuiColor::SetColor("nuiDefaultClrSelection",          nuiColor(56,117,215));

  nuiColor::SetColor("nuiDefaultClrNormalTab",          nuiColor(64,64,64));
  nuiColor::SetColor("nuiDefaultClrSelectedTab",        nuiColor(32,32,32));

  nuiColor::SetColor("nuiDefaultClrProgressFg",         nuiColor(76,145,230));
  nuiColor::SetColor("nuiDefaultClrProgressBg",         nuiColor(200,200,200));
  
  nuiColor::SetColor("nuiDefaultClrPaneBkg",            nuiColor(200,200,200));

  nuiColor::SetColor("nuiDefaultClrBorder",             nuiColor(190,190,190));
  
  nuiColor::SetColor("nuiActiveWindowShade",            nuiColor());
  nuiColor::SetColor("nuiInactiveWindowShade",          nuiColor());

  nuiColor::SetColor("nuiActiveWindowBg",               nuiColor(1.f,1.f,1.f));
  nuiColor::SetColor("nuiInactiveWindowBg",             nuiColor(.9f,.9f,.9f));

  nuiColor::SetColor("nuiActiveWindowFg",               nuiColor(.8f,.8f,1.f));
  nuiColor::SetColor("nuiInactiveWindowFg",             nuiColor(.7f,.7f,.8f));

  nuiColor::SetColor("nuiActiveWindowTitle",            nuiColor(1.f,1.f,1.f));
  nuiColor::SetColor("nuiInactiveWindowTitle",          nuiColor(.8f,.8f,.8f));

  nuiColor::SetColor("nuiSplitterBarFg",                nuiColor(.5f,.5f,.6f));
  nuiColor::SetColor("nuiSplitterBarHover",             nuiColor(.7f,.7f,.8f));
  nuiColor::SetColor("nuiSplitterBarBg",                nuiColor(.5f,.5f,.6f));

  nuiColor::SetColor("nuiScrollBarFg",                  nuiColor(1.f,1.f,1.f, .8f));
  nuiColor::SetColor("nuiScrollBarFgHover",             nuiColor(1.f,1.f,1.f));
  nuiColor::SetColor("nuiScrollBarBg",                  nuiColor(.8f,.8f,1.0f));
  nuiColor::SetColor("nuiScrollBarBgHover",             nuiColor(.5f,.5f,0.6f));

  nuiColor::SetColor("nuiSliderBarFg",                  nuiColor(.5f,.5f,.9f));
  nuiColor::SetColor("nuiSliderBarBg",                  nuiColor(.8f,.8f,1.0f));
  nuiColor::SetColor("nuiSliderBarBgHover",             nuiColor(.5f,.5f,0.6f));

  nuiColor::SetColor("nuiNormalButtonBg",               nuiColor(255, 255, 255, 255));
  nuiColor::SetColor("nuiSelectedButtonBg",             nuiColor(255, 255, 255, 255));
  nuiColor::SetColor("nuiDisabledButtonBg",             nuiColor(255, 255, 255, 255));
  nuiColor::SetColor("nuiHoverNormalButtonBg",          nuiColor(255, 255, 255, 255));
  nuiColor::SetColor("nuiHoverSelectedButtonBg",        nuiColor(255, 255, 255, 255));
  nuiColor::SetColor("nuiHoverDisabledButtonBg",        nuiColor(255, 255, 255, 255));

  nuiColor::SetColor("nuiKnobBg",                       nuiColor(.5f,.5f,0.6f));
  nuiColor::SetColor("nuiKnobMarker",                   nuiColor(.8f,.8f,1.0f));

  nuiColor::SetColor("nuiNormalTabBg",                  nuiColor());
  nuiColor::SetColor("nuiSelectedTabBg",                nuiColor());
  nuiColor::SetColor("nuiDisabledTabBg",                nuiColor());
  nuiColor::SetColor("nuiHoverNormalTabBg",             nuiColor());
  nuiColor::SetColor("nuiHoverSelectedTabBg",           nuiColor());
  nuiColor::SetColor("nuiHoverDisabledTabBg",           nuiColor());

  nuiColor::SetColor("nuiSelectedTableColumnBg",        nuiColor(0.86328125f,0.86328125f,0.86328125f,0.5f));
  nuiColor::SetColor("nuiTableColumnBg",                nuiColor(0.9296875f,0.9296875f,0.9296875f,0.7f));
  nuiColor::SetColor("nuiSelectedTableLineBg",          nuiColor(0.2f,0.2f,0.9f,0.3f));

  nuiColor::SetColor("nuiTreeViewHandle",               nuiColor(0.4f, 0.4f, 0.4f, 0.5f));
  nuiColor::SetColor("nuiTreeViewSelection",            nuiColor(0.4f, 0.5f, 1.f, 0.5f));

  nuiColor::SetColor("nuiSelectionMarkee",              nuiColor(0.8f, 0.8f, 1.f, 0.9f));

  nuiColor::SetColor("nuiNormalTextFg",                 nuiColor(0.f,0.f,0.f));
  nuiColor::SetColor("nuiSelectedTextFg",               nuiColor(1.f,1.f,1.f));
  nuiColor::SetColor("nuiDisabledTextFg",               nuiColor(.5f,.5f,.5f));
  nuiColor::SetColor("nuiNormalTextBg",                 nuiColor(1.0f, 1.0f, 1.0f, 0.0f));
  nuiColor::SetColor("nuiSelectedTextBg",               nuiColor(.8f,.8f,.8f,.5f));
  nuiColor::SetColor("nuiDisabledTextBg",               nuiColor(.3f,.3f,.3f,0.f));

  nuiColor::SetColor("nuiMenuBg",                       nuiColor(1.0f, 1.0f, 1.0f, .9f));
  nuiColor::SetColor("nuiMenuFg",                       nuiColor(0.f, 0.f, 0.f, 0.f));
  nuiColor::SetColor("nuiMenuTitleBg",                  nuiColor(.5f, .5f, .5f));

  nuiColor::SetColor("nuiShapeFill",                    nuiColor(.5f, .5f, .5f, .5f));
  nuiColor::SetColor("nuiShapeStroke",                  nuiColor(0.0f, 0.0f, 0.0f, 0.5f));
  nuiColor::SetColor("nuiBorder",                       nuiColor(.5f, .5f, .5f, 1.f));
  nuiColor::SetColor("nuiDisabledImage",                nuiColor(.5f, .5f, .5f, 1.f));
  nuiColor::SetColor("nuiSelectedImage",                nuiColor(.9f, .9f, .9f, 1.f));

  nuiColor::SetColor("nuiToolTipBg",                    nuiColor(1.f, 1.f, 1.f, .8f));
  nuiColor::SetColor("nuiToolTipFg",                    nuiColor());
  nuiColor::SetColor("nuiToolTipBorder",                nuiColor(0.f, 0.f, 0.f, .3f));

  nuiColor::SetColor("nuiMatrixViewHeaderBorder",       nuiColor(120,120,120));
  nuiColor::SetColor("nuiMatrixViewHeaderBackground",   nuiColor(180,180,180));
  nuiColor::SetColor("nuiMatrixViewBorder",             nuiColor(120,120,120));
  nuiColor::SetColor("nuiMatrixViewBackground",         nuiColor(220,220,220));

}




//**************************************************************************************************************
//
// InitSelection
//
void nuiDefaultDecoration::InitSelection()
{
  nuiColor color;
  nuiColor::GetColor("nuiDefaultClrSelection", color);
  
  nuiColorDecoration* pDeco = new nuiColorDecoration("nuiDefaultDecorationSelectionBackground", 
                                                     nuiRect(3,3,0,0), color);
}




//**************************************************************************************************************
//
// InitMaps
//
// those maps are loaded in decorations, but are used dynamically, 
// during the application running
//
void nuiDefaultDecoration::InitMaps()
{
  // vertical scrollbar background
  nuiTexture* pTex = nuiTexture::GetTexture("ScrollbarVerticalBkg");
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame("nuiDefaultDecorationScrollBarVerticalBkg", pTex, nuiRect(6,8,0,0));
  pFrame->UseWidgetAlpha(true);
  
  // vertical scrollbar handle
  pTex = nuiTexture::GetTexture("ScrollbarVerticalHdl");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationScrollBarVerticalHdl", pTex, nuiRect(2,6,6,0));
  pFrame->UseWidgetAlpha(true);
    
  // Horizontal scrollbar background
  pTex = nuiTexture::GetTexture("ScrollbarHorizontalBkg");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationScrollBarHorizontalBkg", pTex, nuiRect(8,6,0,0));
  pFrame->UseWidgetAlpha(true);
    
  // Horizontal scrollbar handle
  pTex = nuiTexture::GetTexture("ScrollbarHorizontalHdl");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationScrollBarHorizontalHdl", pTex, nuiRect(6,2,0,6));
  pFrame->UseWidgetAlpha(true);
  
  
  
  // mobile scrollbar handle
  pTex = nuiTexture::GetTexture("ScrollbarMobile");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationMobileScrollbarHandle", pTex, nuiRect(5,5,1,1));
  pFrame->UseWidgetAlpha(true);
  
  
  
  
  
  
  // vertical slider background
  pTex = nuiTexture::GetTexture("SliderVerticalBkg");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationSliderVerticalBkg", pTex, nuiRect(0,6,4,0));
  pFrame->UseWidgetAlpha(true);
    
  // vertical slider handle
  pTex = nuiTexture::GetTexture("SliderVerticalHdl");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationSliderVerticalHdl", pTex, nuiRect(0,0,20,20));
  pFrame->UseWidgetAlpha(true);
    
  // Horizontal slider background
  pTex = nuiTexture::GetTexture("SliderHorizontalBkg");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationSliderHorizontalBkg", pTex, nuiRect(6,0,0,4));
  pFrame->UseWidgetAlpha(true);
    
  // Horizontal slider handle
  pTex = nuiTexture::GetTexture("SliderHorizontalHdl");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationSliderHorizontalHdl", pTex, nuiRect(0,0,20,20));
  pFrame->UseWidgetAlpha(true);
  
  
  
  // popup Menu
  pTex = nuiTexture::GetTexture("PopupMenu");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationPopupMenu", pTex, nuiRect(8,8,0,14));
  pFrame->UseWidgetAlpha(true);



  // arrow handle close
  pTex = nuiTexture::GetTexture("ArrowClose");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationArrowClose", pTex, nuiRect(0,0,7,6), nuiColor(0,0,0));
  pFrame->UseWidgetAlpha(true);

  
  
  // arrow handle open
  pTex = nuiTexture::GetTexture("ArrowOpen");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationArrowOpen", pTex, nuiRect(0,0,6,7), nuiColor(0,0,0));
  pFrame->UseWidgetAlpha(true);
  
  
  
  
  // Checkerboard for color alpha viewing
  pTex = nuiTexture::GetTexture("CheckerboardSmall");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationCheckerboardSmall", pTex, nuiRect(0,0,20,20));
  pFrame->UseWidgetAlpha(true);
  

}




//**************************************************************************************************************
//
// InitIcons
//

// static 
std::list<nuiTexture*> nuiDefaultDecoration::mIcons;

void nuiDefaultDecoration::InitIcons()
{
  // volume icon
  nuiTexture* pTex = nuiTexture::GetTexture("IconVolume");
  NGL_ASSERT(pTex);
  pTex->SetSource("nuiDefaultDecorationIconVolume");
  mIcons.push_back(pTex);
  
  // folder icon
  pTex = nuiTexture::GetTexture("IconFolder");
    NGL_ASSERT(pTex);
  pTex->SetSource("nuiDefaultDecorationIconFolder");
  mIcons.push_back(pTex);

  // open folder icon
  pTex = nuiTexture::GetTexture("IconOpenFolder");
    NGL_ASSERT(pTex);
  pTex->SetSource("nuiDefaultDecorationIconOpenFolder");
  mIcons.push_back(pTex);

  // parent folder icon
  pTex = nuiTexture::GetTexture("IconParentFolder");
    NGL_ASSERT(pTex);
  pTex->SetSource("nuiDefaultDecorationIconParentFolder");
  mIcons.push_back(pTex);
  
  // file icon
  pTex = nuiTexture::GetTexture("IconFile");
    NGL_ASSERT(pTex);
  pTex->SetSource("nuiDefaultDecorationIconFile");
  mIcons.push_back(pTex);
}




//**************************************************************************************************************
//
// InitImages
//

// static 
std::map<nglString, std::pair<const char*, int32> > nuiDefaultDecoration::mImages;

void nuiDefaultDecoration::InitImages()
{
  mImages["nuiFileTree::DraggedFileIcon"] = std::make_pair(gpImageDraggedFile, gImageDraggedFileSize);
}


//static 
nglImage* nuiDefaultDecoration::GetImage(const nglString& rRef)
{
  std::map<nglString, std::pair<const char*, int32> >::iterator it = mImages.find(rRef);
  if (it == mImages.end())
    return NULL;
  
  const char* pMem = (const char*)(it->second.first);
  int32 size = it->second.second;
  nglIMemory* pIMem = new nglIMemory(pMem, size);
  nglImage* pImage = new nglImage(pIMem);
  return pImage;
}





//**************************************************************************************************************
//
// nuiLabel
//
void nuiDefaultDecoration::Label(nuiWidget* pWidget)
{
//  nuiColor cText;
//  cText.SetValue("nuiDefaultColorLabelText");
//  pWidget->SetColor(eNormalTextFg, cText);
}


//**************************************************************************************************************
//
// nuiEditText
//
void nuiDefaultDecoration::EditText(nuiWidget* pWidget)
{
  
}


//**************************************************************************************************************
//
// nuiEditLine
//
void nuiDefaultDecoration::EditLine(nuiWidget* pWidget)
{
  nglString decoName = "nuiDefaultDecorationInnerPane";
  nuiRect rect = nuiRect(6,6,0,0);
  
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get(decoName);
  
  if (!pFrame)
  {
    nuiTexture* pTex = nuiTexture::GetTexture("PaneInner");
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame(decoName, pTex, rect);
    pFrame->UseWidgetAlpha(true);
  }
  NGL_ASSERT(pFrame);
  
  pWidget->SetDecoration(pFrame, eDecorationBorder);  
}


//**************************************************************************************************************
//
// nuiSplitter
//
void nuiDefaultDecoration::Splitter(nuiWidget* pWidget)
{
  nuiSplitter* pSplitter = (nuiSplitter*)pWidget;
  nuiSplitterHandle* pHandle = pSplitter->GetHandle();
  NGL_ASSERT(pHandle);
  
  if (pSplitter->GetOrientation() == nuiVertical)
  {
    pHandle->SetUserSize(7,0);
    
    nuiMetaDecoration* pDeco = (nuiMetaDecoration*)nuiDecoration::Get("nuiDefaultDecorationVerticalSplitter");
    if (!pDeco)
    {
      // handle bar 
      nuiTexture* pTex = nuiTexture::GetTexture("SplitterVertical");
      NGL_ASSERT(pTex);
      
      nuiFrame* pFrame = new nuiFrame("nuiDefaultDecorationVerticalSplitterBar", pTex, nuiRect(0,0,7,3));
      pFrame->SetInterpolated(false);
      
      // handle spot
      pTex = nuiTexture::GetTexture("SplitterVerticalHandle");
      NGL_ASSERT(pTex);
      
      nuiImageDecoration* pDeco3 = new nuiImageDecoration("nuiDefaultDecorationVerticalSplitterHandle", pTex, nuiRect(0,0,7,6), nuiCenter);

      // build meta decoration
      pDeco = new nuiMetaDecoration("nuiDefaultDecorationVerticalSplitter");
      pDeco->AddDecoration("nuiDefaultDecorationVerticalSplitterBar");
      pDeco->AddDecoration("nuiDefaultDecorationVerticalSplitterHandle");

    }
    pHandle->SetDecoration(pDeco, eDecorationBorder);  
    
  }
  else
  {
    pHandle->SetUserSize(0,7);
    
    nuiMetaDecoration* pDeco = (nuiMetaDecoration*)nuiDecoration::Get("nuiDefaultDecorationHorizontalSplitter");
    if (!pDeco)
    {
      // handle bar 
      nuiTexture* pTex = nuiTexture::GetTexture("SplitterHorizontal");
      NGL_ASSERT(pTex);
      
      nuiFrame* pFrame = new nuiFrame("nuiDefaultDecorationHorizontalSplitterBar", pTex, nuiRect(0,0,3,7));
      pFrame->SetInterpolated(false);
      
      // handle spot
      pTex = nuiTexture::GetTexture("SplitterHorizontalHandle");
      NGL_ASSERT(pTex);
      
      nuiImageDecoration* pDeco3 = new nuiImageDecoration("nuiDefaultDecorationHorizontalSplitterHandle", 
                                                          pTex, nuiRect(0,0,6,7), nuiCenter);
      
      // build meta decoration
      pDeco = new nuiMetaDecoration("nuiDefaultDecorationHorizontalSplitter");
      pDeco->AddDecoration("nuiDefaultDecorationHorizontalSplitterBar");
      pDeco->AddDecoration("nuiDefaultDecorationHorizontalSplitterHandle");
      
    }
   pHandle->SetDecoration(pDeco, eDecorationBorder);  
  }
  
  
}


//**************************************************************************************************************
//
// nuiTitledPane
//
void nuiDefaultDecoration::TitledPane(nuiWidget* pWidget)
{
  
}


//**************************************************************************************************************
//
// nuiFolderPane
//
void nuiDefaultDecoration::FolderPane(nuiWidget* pWidget)
{
  
}


//**************************************************************************************************************
//
// nuiButton
//
void nuiDefaultDecoration::Button(nuiWidget* pWidget)
{
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get("nuiDefaultDecorationButton");
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nuiTexture* pTexUp = nuiTexture::GetTexture("ButtonUp");
  NGL_ASSERT(pTexUp);
  nuiFrame* pFrameUp = new nuiFrame("nuiDefaultDecorationButtonUp", pTexUp, nuiRect(4,4,2,6));

  
  nuiTexture* pTexHover = nuiTexture::GetTexture("ButtonUp");
  NGL_ASSERT(pTexHover);
  nuiFrame* pFrameHover = new nuiFrame("nuiDefaultDecorationButtonHover", pTexHover, nuiRect(4,4,2,6));
  
  
  nuiTexture* pTexDown = nuiTexture::GetTexture("ButtonDown");
  NGL_ASSERT(pTexDown);
  nuiFrame* pFrameDown = new nuiFrame("nuiDefaultDecorationButtonDown", pTexDown, nuiRect(4,4,2,6));
  
  
  
  nuiStateDecoration* pState = new nuiStateDecoration("nuiDefaultDecorationButton", 
                                                      "nuiDefaultDecorationButtonUp",
                                                      "nuiDefaultDecorationButtonDown",
                                                      "nuiDefaultDecorationButtonHover");
  pState->SetSourceClientRect(nuiRect(4,4,2,6));

  pWidget->SetDecoration(pState, eDecorationBorder);

}





//**************************************************************************************************************
//
// nuiRadioButton
//
void nuiDefaultDecoration::RadioButton(nuiWidget* pWidget)
{
  nuiRadioButton* pBtn = (nuiRadioButton*)pWidget;
  if (pBtn->HasContents())
  {
    nuiDefaultDecoration::Button(pWidget);
    return;
  }
  
  
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get("nuiDefaultDecorationRadioButton");
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nuiTexture* pTex = nuiTexture::GetTexture("RadioButtonUp");
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame("nuiDefaultDecorationRadioButtonUp", pTex, nuiRect(0,0,13,13));
    
  
  pTex = nuiTexture::GetTexture("RadioButtonDown");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationRadioButtonDown", pTex, nuiRect(0,0,13,13));
  
  pTex = nuiTexture::GetTexture("RadioButtonUpDisabled");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationRadioButtonUpDisabled", pTex, nuiRect(0,0,13,13));
  
  pTex = nuiTexture::GetTexture("RadioButtonDownDisabled");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationRadioButtonDownDisabled", pTex, nuiRect(0,0,13,13));
    
  
  nuiStateDecoration* pState = new nuiStateDecoration("nuiDefaultDecorationRadioButton", 
                                                      "nuiDefaultDecorationRadioButtonUp",
                                                      "nuiDefaultDecorationRadioButtonDown",
                                                      "nuiDefaultDecorationRadioButtonUp",/* hover up */
                                                      "nuiDefaultDecorationRadioButtonDown", /* hover down*/
                                                      "nuiDefaultDecorationRadioButtonUpDisabled",
                                                      "nuiDefaultDecorationRadioButtonDownDisabled");
  pState->SetSourceClientRect(nuiRect(0,0,13,13));
  
  pWidget->SetDecoration(pState, eDecorationBorder);
  
}







//**************************************************************************************************************
//
// nuiCloseButton (for windows)
//
void nuiDefaultDecoration::CloseButton(nuiWidget* pWidget)
{
  
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get("nuiDefaultDecorationCloseButton");
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nuiTexture* pTexUp = nuiTexture::GetTexture("CloseButtonUp");
  NGL_ASSERT(pTexUp);
  nuiFrame* pFrameUp = new nuiFrame("nuiDefaultDecorationCloseButtonUp", pTexUp, nuiRect(0,0,12,15));
  
  
  nuiTexture* pTexHover = nuiTexture::GetTexture("CloseButtonHover");
  NGL_ASSERT(pTexHover);
  nuiFrame* pFrameHover = new nuiFrame("nuiDefaultDecorationCloseButtonHover", pTexHover, nuiRect(0,0,12,15));
  
  
  nuiTexture* pTexDown = nuiTexture::GetTexture("CloseButtonDown");
  NGL_ASSERT(pTexDown);
  nuiFrame* pFrameDown = new nuiFrame("nuiDefaultDecorationCloseButtonDown", pTexDown, nuiRect(0,0,12,15));
  
  
  
  nuiStateDecoration* pState = new nuiStateDecoration("nuiDefaultDecorationCloseButton", 
                                                      "nuiDefaultDecorationCloseButtonUp",
                                                      "nuiDefaultDecorationCloseButtonDown",
                                                      "nuiDefaultDecorationCloseButtonHover");
  pState->SetSourceClientRect(nuiRect(0,0,12,15));
  
  pWidget->SetDecoration(pState, eDecorationBorder);
  
}




//**************************************************************************************************************
//
// nuiToggleButton
//
void nuiDefaultDecoration::ToggleButton(nuiWidget* pWidget)
{
  nuiToggleButton* pBtn = (nuiToggleButton*)pWidget;
  if (!pBtn->GetDisplayAsCheckBox() && !pBtn->GetDisplayAsFrameBox())
  {
    nuiDefaultDecoration::Button(pWidget);
    return;
  }
  
  if (pBtn->GetDisplayAsCheckBox())
  {
    nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get("nuiDefaultDecorationToggleButtonCheckBox");
    if (pDeco)
    {
      pWidget->SetDecoration(pDeco, eDecorationBorder);
      return;
    }
    
    nuiTexture* pTex = nuiTexture::GetTexture("ToggleButtonUp");
    NGL_ASSERT(pTex);
    nuiImageDecoration* pFrame = new nuiImageDecoration("nuiDefaultDecorationToggleButtonCheckBoxUp", pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
        
    
    pTex = nuiTexture::GetTexture("ToggleButtonDown");
    NGL_ASSERT(pTex);
    pFrame = new nuiImageDecoration("nuiDefaultDecorationToggleButtonCheckBoxDown", pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
        
    pTex = nuiTexture::GetTexture("ToggleButtonUpDisabled");
    NGL_ASSERT(pTex);
    pFrame = new nuiImageDecoration("nuiDefaultDecorationToggleButtonCheckBoxUpDisabled", pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
        
    pTex = nuiTexture::GetTexture("ToggleButtonDownDisabled");
    NGL_ASSERT(pTex);
    pFrame = new nuiImageDecoration("nuiDefaultDecorationToggleButtonCheckBoxDownDisabled", pTex, nuiRect(0,0,13,13));
    pFrame->SetPosition(nuiLeft);
        
    
    nuiStateDecoration* pState = new nuiStateDecoration("nuiDefaultDecorationToggleButtonCheckBox", 
                                                        "nuiDefaultDecorationToggleButtonCheckBoxUp",
                                                        "nuiDefaultDecorationToggleButtonCheckBoxDown",
                                                        "nuiDefaultDecorationToggleButtonCheckBoxUp",/* hover up */
                                                        "nuiDefaultDecorationToggleButtonCheckBoxDown", /* hover down*/
                                                        "nuiDefaultDecorationToggleButtonCheckBoxUpDisabled",
                                                        "nuiDefaultDecorationToggleButtonCheckBoxDownDisabled");
    pState->SetSourceClientRect(nuiRect(0,0,13,13));
    pWidget->SetDecoration(pState, eDecorationBorder);
  }
  
}



//**************************************************************************************************************
//
// nuiComboBox
//
void nuiDefaultDecoration::ComboBox(nuiWidget* pWidget)
{
 
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get("nuiDefaultDecorationComboBox");
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nuiTexture* pTex = nuiTexture::GetTexture("ComboUp");
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame("nuiDefaultDecorationComboBoxUp", pTex, nuiRect(12,4,2,10));
  pFrame->EnableBorder(false);
      
  pTex = nuiTexture::GetTexture("ComboDown");
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame("nuiDefaultDecorationComboBoxDown", pTex, nuiRect(12,4,2,10));
  pFrame->EnableBorder(false);
    
  
  
  nuiStateDecoration* pState = new nuiStateDecoration("nuiDefaultDecorationComboBox");
  pState->SetState(nuiStateEnabled  | nuiStateReleased, nuiDecoration::Get("nuiDefaultDecorationComboBoxUp"));
  pState->SetState(nuiStateEnabled  | nuiStateSelected, nuiDecoration::Get("nuiDefaultDecorationComboBoxDown"));
  pWidget->SetDecoration(pState, eDecorationBorder);
  pWidget->SetUserHeight(18);
  
//  pWidget->SetColor(eSelectedTextFg, nuiColor(32,32,32));

}



//**************************************************************************************************************
//
// nuiKnob
//

void nuiDefaultDecoration::KnobSequence(nuiWidget* pWidget)
{
}




//***********************************************************************************************************************************
//***********************************************************************************************************************************
//
// FileSelector
//
//***********************************************************************************************************************************
//***********************************************************************************************************************************

void nuiDefaultDecoration::FileSelector_FolderView(nuiWidget* pWidget)
{
  nuiColorDecoration* pDeco = (nuiColorDecoration*)nuiDecoration::Get("nuiDefaultDecorationFileSelector_FolderView");
  if (!pDeco)
  {
    pDeco = new nuiColorDecoration("nuiDefaultDecorationFileSelector_FolderView", nuiRect(5,5,0,0), nuiColor(214,221,229), 1, nuiColor(139,139,139), eStrokeAndFillShape);
  }
  pWidget->SetDecoration(pDeco, eDecorationBorder);
}

void nuiDefaultDecoration::FileSelector_InfoView(nuiWidget* pWidget)
{

}


void nuiDefaultDecoration::FileSelector_FolderLabel(nuiWidget* pWidget)
{
  
}


void nuiDefaultDecoration::FileSelector_VolumeIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconVolume"));
  NGL_ASSERT(pTex);

  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
  
}


void nuiDefaultDecoration::FileSelector_FolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconFolder"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}



void nuiDefaultDecoration::FileSelector_TreeView(nuiWidget* pWidget)
{
  nuiColorDecoration* pDeco = (nuiColorDecoration*)nuiDecoration::Get("nuiDefaultDecorationFileSelectorWindow");
  if (!pDeco)
  {
    pDeco = new nuiColorDecoration("nuiDefaultDecorationFileSelectorWindow", nuiColor(250,250,250), 1, nuiColor(175,175,175), eStrokeAndFillShape);
  }
  pWidget->SetDecoration(pDeco, eDecorationBorder);  
  
}


void nuiDefaultDecoration::FileSelector_TreeFolderLabel(nuiWidget* pWidget)
{
  
}


void nuiDefaultDecoration::FileSelector_TreeFileLabel(nuiWidget* pWidget)
{
  
}


void nuiDefaultDecoration::FileSelector_TreeFolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconFolder"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileSelector_TreeFileIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconFile"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}
















//***********************************************************************************************************************************
//***********************************************************************************************************************************
//
// FileTree
//
//***********************************************************************************************************************************
//***********************************************************************************************************************************

void nuiDefaultDecoration::FileTree_View(nuiWidget* pWidget)
{
  nuiGradientDecoration* pDeco = (nuiGradientDecoration*)nuiDecoration::Get("nuiDefaultDecorationWindow");
  if (!pDeco)
  {
    nuiColor color1, color2;
    nuiColor::GetColor("nuiDefaultClrWindowBkg1", color1);
    nuiColor::GetColor("nuiDefaultClrWindowBkg2", color2);
    
    pDeco = new nuiGradientDecoration("nuiDefaultDecorationWindow", 
                                      nuiRect(0,0, 0,0), color1, color2, nuiVertical, 1, nuiColor(175,175,175), eStrokeAndFillShape);
    pDeco->SetOffset1(0.f);
    pDeco->SetOffset2(0.5f);
  }
  pWidget->SetDecoration(pDeco, eDecorationBorder);  
}


//void nuiDefaultDecoration::FileTree_Node(nuiWidget* pWidget)
//{
//  nuiBorderDecoration* pDeco = (nuiBorderDecoration*)nuiDecoration::Get("nuiDefaultDecorationFileTreeNode");
//  if (!pDeco)
//  {
//    nuiColor color;
//    nuiColor::GetColor("nuiDefaultClrBorder", color);
//    
//    pDeco = new nuiBorderDecoration("nuiDefaultDecorationFileTreeNode");
//    pDeco->SetSourceClientRect(nuiRect(0,0,0,0));
//    pDeco->SetStrokeColor(color);
//    pDeco->SetStrokeSize(1);
//    pDeco->SetBorderType("Bottom");
//  }
//  
//  pWidget->SetDecoration(pDeco, eDecorationBorder);  
//}
//

void nuiDefaultDecoration::FileTree_ColumnLabel(nuiWidget* pWidget)
{  
  pWidget->SetPosition(nuiFillHorizontal);
  
  nuiColorDecoration* pDeco = (nuiColorDecoration*)nuiDecoration::Get("nuiDefaultDecorationFileTreeColumnLabel");
  if (!pDeco)
  {
    nuiColor fillColor;
    nuiColor::GetColor("nuiDefaultClrPaneBkg", fillColor);
    pDeco = new nuiColorDecoration("nuiDefaultDecorationFileTreeColumnLabel", nuiRect(3,3,0,0), fillColor);
  }
  
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(10), true);
  
  pWidget->SetDecoration(pDeco, eDecorationBorder);
}


void nuiDefaultDecoration::FileTree_FolderLabel(nuiWidget* pWidget)
{  
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(10), true);
}

void nuiDefaultDecoration::FileTree_FileLabel(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(10), true);  
}

void nuiDefaultDecoration::FileTree_FileInfo(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  pLabel->SetFont(nuiFont::GetFont(8), true);
}


void nuiDefaultDecoration::FileTree_VolumeIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconVolume"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileTree_FolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconFolder"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileTree_OpenFolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconOpenFolder"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}


void nuiDefaultDecoration::FileTree_ParentFolderIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconParentFolder"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}



void nuiDefaultDecoration::FileTree_FileIcon(nuiWidget* pWidget)
{
  nuiTexture* pTex = nuiTexture::GetTexture(nglString("nuiDefaultDecorationIconFile"));
  NGL_ASSERT(pTex);
  
  NGL_ASSERT(dynamic_cast<nuiImage*>(pWidget));
  nuiImage* pImg = (nuiImage*)pWidget;
  pImg->SetTexture(pTex);
  pWidget->InvalidateLayout();
}





//***********************************************************************************************************************************
//***********************************************************************************************************************************
//
// special cases. Those are not called through the automatic default decoration system.
// they are called from their own source code.
//
//***********************************************************************************************************************************
//***********************************************************************************************************************************


//**************************************************************************************************************
//
// nuiMainWindow
//
void nuiDefaultDecoration::MainWindow(nuiMainWindow* pWindow)
{
//  nuiWidget* pCont = new nuiWidget();
//  pWindow->AddChild(pCont);
  nuiColor color1, color2;
  nuiColor::GetColor("nuiDefaultClrWindowBkg1", color1);
  nuiColor::GetColor("nuiDefaultClrWindowBkg2", color2);
  nuiGradientDecoration* pDeco = new nuiGradientDecoration("nuiDefaultDecorationMainWindow", 
                                                           nuiRect(0,0, 0,0), color1, color2, nuiVertical, 0, nuiColor(0,0,0), eFillShape);
  pDeco->SetOffset1(0.f);
  pDeco->SetOffset2(0.5f);                                                           
  pWindow->SetDecoration(pDeco);  
}


//**************************************************************************************************************
//
// nuiDialog
//
void nuiDefaultDecoration::Dialog(nuiWidget* pCont)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get("nuiDefaultDecorationOutterPane");
  if (!pFrame)
  {
    nuiTexture* pTex = nuiTexture::GetTexture("PaneOutter");
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame("nuiDefaultDecorationOutterPane", pTex, nuiRect(12,12,0,1));
    pFrame->UseWidgetAlpha(true);
  }
  NGL_ASSERT(pFrame);
  pCont->SetDecoration(pFrame);  
}


//**************************************************************************************************************
//
// nuiDialog::Title
//
void nuiDefaultDecoration::Dialog_Title(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  
  pLabel->SetFont(nuiFont::GetFont(13), true);
  pLabel->SetBorder(0,0,10,15);
}

//**************************************************************************************************************
//
// nuiDialog::EditLine
//
void nuiDefaultDecoration::Dialog_EditLine(nuiWidget* pWidget)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get("nuiDefaultDecorationInnerPane");
  if (!pFrame)
  {
    nuiTexture* pTex = nuiTexture::GetTexture("PaneInner");
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame("nuiDefaultDecorationInnerPane", pTex, nuiRect(6,6,0,0));
    pFrame->UseWidgetAlpha(true);
  }
  NGL_ASSERT(pFrame);
  pWidget->SetDecoration(pFrame, eDecorationBorder);    
}



//**************************************************************************************************************
//
// nuiMessageBox
//
void nuiDefaultDecoration::MessageBox(nuiMessageBox* pBox)
{
  nuiFrame* pFrame = (nuiFrame*)nuiDecoration::Get("nuiDefaultDecorationOutterPane");
  if (!pFrame)
  {
    nuiTexture* pTex = nuiTexture::GetTexture("PaneOutter");
    NGL_ASSERT(pTex);
    pFrame = new nuiFrame("nuiDefaultDecorationOutterPane", pTex, nuiRect(12,12,0,1));
    pFrame->UseWidgetAlpha(true);
  }
  NGL_ASSERT(pFrame);
  pBox->SetDecoration(pFrame);
}



//**************************************************************************************************************
//
// nuiMessageBox::Title
//
void nuiDefaultDecoration::MessageBox_Title(nuiWidget* pWidget)
{
  nuiLabel* pLabel = (nuiLabel*)pWidget;
  
  pLabel->SetFont(nuiFont::GetFont(14), true);
  pLabel->SetBorder(0,0,0,15);
}



//**************************************************************************************************************
//
// nuiTabView::Tab
//
void nuiDefaultDecoration::TabView_Tab(nuiTabView* pView, nuiWidget* pTab)
{
  nglString decoName = "nuiDefaultDecorationTabTop";
  nglString decoUpName;
  nglString decoDownName;
  
  nuiRect frameRect;

  const char* decoUp = "TabTopUp";
  const char* decoDown = "TabTopDown";
  
  switch (pView->GetTabPosition())
  {
    case nuiTop:
      decoName = "nuiDefaultDecorationTabTop";
      decoUp = "TabTopUp";
      decoDown = "TabTopDown";
      frameRect = nuiRect(4,5,2,4);
      break;
      
    case nuiLeft:
      decoName = "nuiDefaultDecorationTabLeft";
      decoUp = "TabLeftUp";
      decoDown = "TabLeftDown";
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiRight:
      decoName = "nuiDefaultDecorationTabRight";
      decoUp = "TabRightUp";
      decoDown = "TabRightDown";
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiBottom:
      decoName = "nuiDefaultDecorationTabBottom";
      decoUp = "TabBottomUp";
      decoDown = "TabBottomDown";
      frameRect = nuiRect(4,5,2,4);
      break;
      
    default:
      NGL_OUT("nui3 error : the nuiTabView object doesn't have a nuiPosition valid parameter!\n");
      NGL_ASSERT(0);
      break;
  }
  
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(decoName);
  if (pDeco)
  {
    pTab->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  decoUpName = decoName + "Up";
  decoDownName = decoName + "Down";
  
  nuiTexture* pTex = nuiTexture::GetTexture(decoUp);
  NGL_ASSERT(pTex);
  nuiFrame* pFrame = new nuiFrame(decoUpName, pTex, frameRect);
    
  pTex = nuiTexture::GetTexture(decoDown);
  NGL_ASSERT(pTex);
  pFrame = new nuiFrame(decoDownName, pTex, frameRect);
    
  
  
  nuiStateDecoration* pState = new nuiStateDecoration(decoName);
  pState->SetState(nuiStateEnabled  | nuiStateReleased, nuiDecoration::Get(decoUpName));
  pState->SetState(nuiStateEnabled  | nuiStateSelected, nuiDecoration::Get(decoDownName));
  
  pState->SetSourceClientRect(frameRect);
  
  pTab->SetDecoration(pState, eDecorationBorder);
  
}





//**************************************************************************************************************
//
// nuiTabView::Contents
//

void nuiDefaultDecoration::TabView_Contents(nuiTabView* pView, nuiWidget* pContents)
{
  nglString decoName = "nuiDefaultDecorationTabTopContents";
  
  nuiRect frameRect;
  
  const char* deco = "TabTopContents";
  
  switch (pView->GetTabPosition())
  {
    case nuiTop:
      decoName = "nuiDefaultDecorationTabTopContents";
      deco = "TabTopContents";
      frameRect = nuiRect(4,5,2,4);
      break;
      
    case nuiLeft:
      decoName = "nuiDefaultDecorationTabLeftContents";
      deco = "TabLeftContents";
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiRight:
      decoName = "nuiDefaultDecorationTabRightContents";
      deco = "TabRightContents";
      frameRect = nuiRect(5,4,4,2);
      break;
      
    case nuiBottom:
      decoName = "nuiDefaultDecorationTabBottomContents";
      deco = "TabBottomContents";
      frameRect = nuiRect(4,5,2,4);
      break;

    default:
      NGL_ASSERT(0);
      break;
  }
  
  nuiFrame* pDeco = (nuiFrame*)nuiDecoration::Get(decoName);
  if (pDeco)
  {
    pContents->SetDecoration(pDeco, eDecorationBorder);
    return;
  }
  
  nuiTexture* pTex = nuiTexture::GetTexture(deco);
  NGL_ASSERT(pTex);
  pDeco = new nuiFrame(decoName, pTex, frameRect);
    
  pContents->SetDecoration(pDeco, eDecorationBorder);
  
}




//**************************************************************************************************************
//
// NavigationViews
//

//static 
void nuiDefaultDecoration::NavigationBar(nuiWidget* pWidget)
{
  nuiNavigationBar* pBar = (nuiNavigationBar*)pWidget;
  if (!pBar->IsVisible())
    return;
  
  // TODO : plateform? orientation?  
  
  nuiMetaDecoration* pMeta = (nuiMetaDecoration*)nuiDecoration::Get("nuiDefaultNavigationBarDecoration");
  if (pMeta)
  {
    pBar->SetDecoration(pMeta);
    return;
  }
  
  pMeta = new nuiMetaDecoration("nuiDefaultNavigationBarDecoration");
  
  nuiGradientDecoration* pGradient = new nuiGradientDecoration("nuiDefaultNavigationBarDecoration_Gradient");
  pGradient->SetShapeMode(eFillShape);
  
  // TODO : plateform? 
  if (pBar->GetBarStyle() == eBarStyleDefault)
  {
    pGradient->SetColors(nuiColor(145,165,191), nuiColor(109,131,161));

  }
  else if (pBar->GetBarStyle() == eBarStyleBlack)
  {
    pGradient->SetColors(nuiColor(0,0,0), nuiColor(0,0,0));    
  }
//  else
//  {
//    nuiColor tint = pBar->GetTintColor();
//    nuiColor tint2 = nuiColor(tint.Red()+(18.f/255.f), tint.Green()+(17.f/255.f), tint.Blue()+(15.f/255.f), tint.Alpha());
//    pGradient->SetColors(tint2, tint);  
//  }

  
  pMeta->AddDecoration(pGradient);
  
  nuiBorderDecoration* pBorder = new nuiBorderDecoration("nuiDefaultNavigationBarDecoration_BorderTop");
  pBorder->SetBorderType("Top");
  pBorder->SetStrokeTopColor(nuiColor(255,255,255,140));
  pMeta->AddDecoration(pBorder);
  
  pBorder = new nuiBorderDecoration("nuiDefaultNavigationBarDecoration_BorderBottom");
  pBorder->SetBorderType("Bottom");
  pBorder->SetStrokeBottomColor(nuiColor(0,0,0,164));
  pMeta->AddDecoration(pBorder);
  
  pBar->SetDecoration(pMeta);  
}


//static 
void nuiDefaultDecoration::NavigationButton(nuiNavigationButton* pWidget, nuiNavigationBarStyle style, bool leftyButton)
{
  nglString decoName;
  nglString barStyle = NavigationBarStyleToString(style);
  
  decoName.Format("nuiDefaultNavigation%sButtonDecoration", barStyle.GetChars());
  nuiStateDecoration* pDeco = (nuiStateDecoration*)nuiDecoration::Get(decoName);
  
  if (pDeco)
  {
    pWidget->SetDecoration(pDeco);
    return;
  }
  
  // choose the right map, depending on the style of the navigation bar, and on the button kind
  const char* pButtonUp = NULL;
  const char* pButtonDown = NULL;
  if (leftyButton)
    switch (style)
    {
      case eBarStyleDefault: pButtonUp = "NavigationBarDefaultLeftButtonUp"; pButtonDown = "NavigationBarDefaultLeftButtonDown"; break;
      case eBarStyleBlack: pButtonUp = "NavigationBarBlackLeftButtonUp"; pButtonDown = "NavigationBarBlackLeftButtonDown"; break;
      //case eBarStyleTint: return "Tint";
      case eBarStyleNone: default: break;
    }
  else
    switch (style)
    {
      case eBarStyleDefault: pButtonUp = "NavigationBarDefaultButtonUp"; pButtonDown = "NavigationBarDefaultButtonDown"; break;
      case eBarStyleBlack: pButtonUp = "NavigationBarBlackButtonUp"; pButtonDown = "NavigationBarBlackButtonDown"; break;
        //case eBarStyleTint: return "Tint";
      case eBarStyleNone: default: break;
    }


  // assign the texture for the "Up" state
  nglString decoUpName;
  if (leftyButton)
    decoUpName.Format("nuiDefaultNavigation%sLeftButtonUp", barStyle.GetChars());
  else
    decoUpName.Format("nuiDefaultNavigation%sButtonUp", barStyle.GetChars());

  nuiTexture* pTexUp = nuiTexture::GetTexture("ButtonUp");
  NGL_ASSERT(pTexUp);
  nuiFrame* pFrameUp = new nuiFrame(decoUpName, pTexUp, nuiRect(13,0,1,30));
  

  // assign the texture for the "Down" state
  nglString decoDownName;
  if (leftyButton)
    decoDownName.Format("nuiDefaultNavigation%sLeftButtonDown", barStyle.GetChars());
  else
    decoDownName.Format("nuiDefaultNavigation%sButtonDown", barStyle.GetChars());
  
  nuiTexture* pTexDown = nuiTexture::GetTexture("ButtonDown");
  NGL_ASSERT(pTexDown);
  nuiFrame* pFrameDown = new nuiFrame(decoDownName, pTexDown, nuiRect(13,0,1,30));
  
  
  // create "state" decoration for 2-states button
  nuiStateDecoration* pState = new nuiStateDecoration(decoName, decoUpName, decoDownName);
  pState->SetSourceClientRect(nuiRect(13,0,1,30));
  
  // set decoration
  pWidget->SetDecoration(pState, eDecorationBorder);
  
  // set style
  pWidget->SetPosition(nuiLeft);
  pWidget->SetBorder(20, 0, 7, 0);
  
}




//static
nglString nuiDefaultDecoration::NavigationBarStyleToString(nuiNavigationBarStyle barStyle)
{
  switch (barStyle)
  {
    case eBarStyleDefault: return "Default";
    case eBarStyleBlack: return "Black";
      //case eBarStyleTint: return "Tint";
    case eBarStyleNone: return "None";
  }
  return "UnknownStyle";
}





