//
//  nuiNode.h
//  nui3
//
//  Created by Sebastien Metrot on 23/02/15.
//  Copyright (c) 2015 libNUI. All rights reserved.
//

#pragma once

class nuiNode : public nuiObject
{
public:
  nuiNode(const nglString& rName = nglString::Null);

  void AddChild(nuiNode* pChild);
  void DelChild(nuiNode* pChild);
  nuiNode* GetParent() const;

  /** @name Matrix Transformation Support */
  //@{
  void AddMatrixNode(nuiMatrixNode* pNode);
  void DelMatrixNode(uint32 index);
  int32 GetMatrixNodeCount() const;
  nuiMatrixNode* GetMatrixNode(uint32 index) const;

  void LoadIdentityMatrix(); ///< Beware: you can't use Get/SetPosition/Angle/X or Y after that as the matrixes they use will gone.
  bool IsMatrixIdentity() const;
  void GetMatrix(nuiMatrix& rMatrix) const;
  nuiMatrix GetMatrix() const;
  void SetMatrix(const nuiMatrix& rMatrix);
  //@}

  void CallDraw(nuiDrawContext* pContext);
  virtual void Draw(nuiDrawContext* pContext);

  void SetPosition(float X, float Y);
  void SetX(float X);
  void SetY(float Y);
  float GetX() const;
  float GetY() const;

  float GetAngle() const;
  void SetAngle(float angle);
  const nglVectorf& GetPivot() const;
  void SetPivot(const nglVectorf& rPivot);


  float GetScaleX() const;
  float GetScaleY() const;
  float GetScale() const;
  void SetScaleX(float value);
  void SetScaleY(float value);
  void SetScale(float value);

  void SetColor(const nuiColor& rColor);
  const nuiColor& GetColor() const;

  void SetAlpha(float Alpha);
  float GetAlpha() const;

  void SetBlendFunc(nuiBlendFunc f);
  nuiBlendFunc GetBlendFunc() const;
  void SetBlending(bool value);
  bool GetBlending() const;

  void GetNodesAtPoint(float x, float y, std::vector<nuiNode*>& rNodes);
  virtual bool IsInside(float x, float y) const;

protected:
  virtual ~nuiNode();

  void Init();
  void InitAttributes();

  static nuiMatrix mIdentityMatrix;
  std::vector<nuiMatrixNode*>* mpMatrixNodes;
  std::vector<nuiNode*> mpChildren;
  void SetParent(nuiNode* pParent);

  nuiNode* mpParent;
  int32 mCurrentAnimation;
  float mCurrentFrame;
  float mSpeed;
  float mScale;
  float mScaleX;
  float mScaleY;

  nuiMatrixNode_Scale* mpScale;
  nuiMatrixNode_Translation* mpPosition;
  nuiMatrixNode_Pivot* mpPivot;

  nuiColor mColor;
  float mAlpha;
  nuiBlendFunc mBlendFunc;
  bool mBlending;

private:
  static uint32 mNodeCounter;
  
};

