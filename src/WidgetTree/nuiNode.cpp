//
//  nuiNode.cpp
//  nui3
//
//  Created by Sebastien Metrot on 23/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#include "nui.h"


nuiMatrix nuiNode::mIdentityMatrix;

// static
uint32 nuiNode::mNodeCounter = 0;


nuiNode::nuiNode(const nglString& rName)
: nuiObject(rName), mColor(255, 255, 255), mAlpha(1.0f), mBlendFunc(nuiBlendTransp), mBlending(true)
{
  Init();
}


nuiNode::~nuiNode()
{
  LoadIdentityMatrix();

  // static counter
  mNodeCounter--;

  for (size_t i = 0; i < mpChildren.size(); i++)
    mpChildren[i]->Release();
}

void nuiNode::Init()
{
  CheckValid();
  if (SetObjectClass(_T("nuiNode")))
  {
    InitAttributes();
  }

  // static counter
  mNodeCounter++;

  mpParent = NULL;
  mpMatrixNodes = NULL;
  mCurrentAnimation = 0;
  mCurrentFrame = 0;
  mSpeed = 1.0f;
  mScale = 1.0f;
  mScaleX = 1.0f;
  mScaleY = 1.0f;

  // Init Matrixes:
  mpScale = new nuiMatrixNode_Scale();
  mpPosition = new nuiMatrixNode_Translation();
  mpPivot = new nuiMatrixNode_Pivot();
  AddMatrixNode(mpPosition);
  AddMatrixNode(mpPivot);
  AddMatrixNode(mpScale);
}

void nuiNode::InitAttributes()
{
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("X")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetX),
                nuiMakeDelegate(this, &nuiNode::SetX)));
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("Y")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetY),
                nuiMakeDelegate(this, &nuiNode::SetY)));

  AddAttribute(new nuiAttribute<float>
               (nglString(_T("Angle")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetAngle),
                nuiMakeDelegate(this, &nuiNode::SetAngle)));

  AddAttribute(new nuiAttribute<float>
               (nglString(_T("Scale")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetScale),
                nuiMakeDelegate(this, &nuiNode::SetScale)));
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("ScaleX")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetScaleX),
                nuiMakeDelegate(this, &nuiNode::SetScaleX)));
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("ScaleY")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetScaleY),
                nuiMakeDelegate(this, &nuiNode::SetScaleY)));
  AddAttribute(new nuiAttribute<const nuiColor&>
               (nglString(_T("Color")), nuiUnitColor,
                nuiMakeDelegate(this, &nuiNode::GetColor),
                nuiMakeDelegate(this, &nuiNode::SetColor)));
  AddAttribute(new nuiAttribute<float>
               (nglString(_T("Alpha")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetAlpha),
                nuiMakeDelegate(this, &nuiNode::SetAlpha)));
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("Blending")), nuiUnitCustom,
                nuiMakeDelegate(this, &nuiNode::GetBlending),
                nuiMakeDelegate(this, &nuiNode::SetBlending)));

}

void nuiNode::AddMatrixNode(nuiMatrixNode* pNode)
{
  CheckValid();
  if (!mpMatrixNodes)
    mpMatrixNodes = new std::vector<nuiMatrixNode*>;

  pNode->Acquire();
  mpMatrixNodes->push_back(pNode);
}

void nuiNode::DelMatrixNode(uint32 index)
{
  if (!mpMatrixNodes)
    return;

  CheckValid();
  mpMatrixNodes->at(index)->Release();
  mpMatrixNodes->erase(mpMatrixNodes->begin() + index);

  DebugRefreshInfo();
}


int32 nuiNode::GetMatrixNodeCount() const
{
  CheckValid();
  if (!mpMatrixNodes)
    return 0;
  return (int32)mpMatrixNodes->size();
}


nuiMatrixNode* nuiNode::GetMatrixNode(uint32 index) const
{
  CheckValid();
  if (mpMatrixNodes)
    return mpMatrixNodes->at(index);
  return NULL;
}


void nuiNode::LoadIdentityMatrix()
{
  CheckValid();

  if (mpMatrixNodes)
  {
    for (size_t i = 0; i < mpMatrixNodes->size(); i++)
      mpMatrixNodes->at(i)->Release();
    delete mpMatrixNodes;
    mpMatrixNodes = NULL;
  }

  DebugRefreshInfo();
}

bool nuiNode::IsMatrixIdentity() const
{
  CheckValid();
  nuiMatrix m;
  GetMatrix(m);
  return !mpMatrixNodes || m.IsIdentity();
}

void nuiNode::GetMatrix(nuiMatrix& rMatrix) const
{
  CheckValid();
  rMatrix.SetIdentity();
  for (size_t i = 0; i < mpMatrixNodes->size(); i++)
    mpMatrixNodes->at(i)->Apply(rMatrix);
}

nuiMatrix nuiNode::GetMatrix() const
{
  CheckValid();
  nuiMatrix m;
  GetMatrix(m);
  return m;
}

void nuiNode::AddChild(nuiNode* pChild)
{
  CheckValid();
  pChild->Acquire();
  nuiNode* pParent = pChild->GetParent();
  if (pParent)
    pParent->DelChild(pChild);
  mpChildren.push_back(pChild);
}

void nuiNode::DelChild(nuiNode* pChild)
{
  CheckValid();
  for (size_t i = 0; i < mpChildren.size(); i++)
  {
    if (mpChildren[i] == pChild)
    {
      mpChildren.erase(mpChildren.begin() + i);
      pChild->Release();
      return;
    }
  }
}

void nuiNode::SetParent(nuiNode* pParent)
{
  CheckValid();
  mpParent = pParent;
}

nuiNode* nuiNode::GetParent() const
{
  CheckValid();
  return mpParent;
}

void nuiNode::CallDraw(nuiDrawContext* pContext)
{
  CheckValid();
  nuiMatrix m;
  GetMatrix(m);
  pContext->PushMatrix();
  pContext->MultMatrix(m);

  pContext->EnableBlending(mBlending);
  pContext->SetBlendFunc(mBlendFunc);
  nuiColor c = mColor;
  c.Multiply(mAlpha);
  pContext->SetFillColor(c);

  Draw(pContext);

  for (size_t i = 0; i < mpChildren.size(); i++)
    mpChildren[i]->Draw(pContext);

  pContext->PopMatrix();
}


void nuiNode::Draw(nuiDrawContext* pContext)
{
}

void nuiNode::SetPosition(float X, float Y)
{
  CheckValid();
  mpPosition->Set(X, Y, 0.0f);
}

void nuiNode::SetAngle(float angle)
{
  CheckValid();
  mpPivot->SetAngle(angle);
}

const nglVectorf& nuiNode::GetPivot() const
{
  CheckValid();
  return mpPivot->GetPivot();
}

void nuiNode::SetPivot(const nglVectorf& rPivot)
{
  CheckValid();
  mpPivot->SetPivot(rPivot);
}



void nuiNode::SetX(float X)
{
  CheckValid();
  mpPosition->SetX(X);
}

void nuiNode::SetY(float Y)
{
  CheckValid();
  mpPosition->SetY(Y);
}

float nuiNode::GetX() const
{
  CheckValid();
  return mpPosition->GetX();
}

float nuiNode::GetY() const
{
  CheckValid();
  return mpPosition->GetY();
}

float nuiNode::GetAngle() const
{
  CheckValid();
  return mpPivot->GetAngle();
}



float nuiNode::GetScaleX() const
{
  CheckValid();
  return mpScale->GetX();
}

float nuiNode::GetScaleY() const
{
  CheckValid();
  return mpScale->GetY();
}

float nuiNode::GetScale() const
{
  CheckValid();
  return mpScale->GetScale();
}

void nuiNode::SetScaleX(float value)
{
  CheckValid();
  mpScale->SetX(value);
}

void nuiNode::SetScaleY(float value)
{
  CheckValid();
  mpScale->SetY(value);
}

void nuiNode::SetScale(float value)
{
  CheckValid();
  mpScale->SetScale(value);
}

void nuiNode::SetColor(const nuiColor& rColor)
{
  CheckValid();
  mColor = rColor;
}

const nuiColor& nuiNode::GetColor() const
{
  CheckValid();
  return mColor;
}

float nuiNode::GetAlpha() const
{
  float v = mAlpha;
  return v;
}

void nuiNode::SetAlpha(float value)
{
  mAlpha = value;
}

void nuiNode::SetBlendFunc(nuiBlendFunc f)
{
  CheckValid();
  mBlendFunc = f;
}

nuiBlendFunc nuiNode::GetBlendFunc() const
{
  CheckValid();
  return mBlendFunc;
}

void nuiNode::SetBlending(bool value)
{
  CheckValid();
  mBlending = value;
}

bool nuiNode::GetBlending() const
{
  CheckValid();
  return mBlending;
}

bool nuiNode::IsInside(float x, float y) const
{
  return false;
}


void nuiNode::GetNodesAtPoint(float x, float y, std::vector<nuiNode*>& rNodes)
{
  CheckValid();
  nuiVector ov(x, y, 0);
  nuiMatrix m;
  GetMatrix(m);
  m.Invert();
  nuiVector v = m * ov;
  x = v[0];
  y = v[1];

  if (IsInside(x, y))
    rNodes.push_back(this);

  uint32 s = mpChildren.size();
  for (size_t i = 0; i < s; i++)
  {
    mpChildren[i]->GetNodesAtPoint(x, y, rNodes);
  }
}

