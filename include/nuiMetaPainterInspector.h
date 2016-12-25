//
//  nuiMetaPainterInspector.h
//  nui3
//
//  Created by Sebastien Metrot on 24/12/2016.
//  Copyright © 2016 libNUI. All rights reserved.
//

#pragma once

class nuiMetaPainterInspector : public nuiWidget
{
public:
  nuiMetaPainterInspector()
  {
    mpBox = new nuiVBox();
    AddChild(mpBox);
  }

  void SetTarget(const nuiWidget* pTarget)
  {
    if (!pTarget)
      return;

    nuiMetaPainter* pPainter = pTarget->GetRenderCache();
    SetTarget(pPainter);
  }

  void SetTarget(const nuiMetaPainter* pPainter)
  {
    mpBox->Clear();
    if (!pPainter)
      return;

    int32 count = pPainter->GetNbOperations();
    for (int32 i = 0; i < count; i++)
    {
      nglString str;
      str = pPainter->GetOperationDescription(i);
      mpBox->AddCell(new nuiLabel(str));
    }
  }
  
private:
  ~nuiMetaPainterInspector() {};
  nuiMetaPainter* mpPainter;
  nuiVBox* mpBox;
};

