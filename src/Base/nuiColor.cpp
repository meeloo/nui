/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"


bool nuiColor::SetValue(const nglString& rString)
{
  const float ratio = 1.0f / 255.0f;
  // Search custom colors first:
  ColorMap::iterator it = mCustomColors.find(rString);
  if (it != mCustomColors.end())
  {
    *this = it->second;
    return true;
  }

  if (rString.GetChar(0)=='#')
  {
    nglString str(rString.Extract(1));

    uint col,len;
    col = str.GetUInt(/*base=*/16);
    len = str.GetLength();
    mRed   = ((col>>16) & 0xFF) * ratio;
    mGreen = ((col>>8 ) & 0xFF) * ratio;
    mBlue  = ((col    ) & 0xFF) * ratio;
    mAlpha = 1;
    if (6 < len)
    {
      // If there is an alpha component get it, otherwise it's 1
      mAlpha = ((col>>24) & 0xFF) * ratio;
      Multiply(mAlpha, false);
    }
    return true;
  }
  else if (rString.Extract(0,3) == "rgb")
  {
    nglString str = rString.Extract(rString.Find('(')+1);
    str.TrimRight(')');
    std::vector<nglString> tokens;
    str.Tokenize(tokens, _T(','));
    
    for (uint32 i = 0; i < tokens.size(); i++)
      tokens[i].Trim();
    
    switch (tokens.size())
    {
    case 4:
      // I'm not sure alpha is in the standard or even handled this way (if you know better, tell me):
      mAlpha = (tokens[3].GetCInt() * ratio);
      mRed   = mAlpha * (tokens[0].GetCInt() * ratio);
      mGreen = mAlpha * (tokens[1].GetCInt() * ratio);
      mBlue  = mAlpha * (tokens[2].GetCInt() * ratio);
      return true;
    case 3:
      mAlpha = 1;
      mRed   = (tokens[0].GetCInt() * ratio);
      mGreen = (tokens[1].GetCInt() * ratio);
      mBlue  = (tokens[2].GetCInt() * ratio);
      return true;
      break;
    default:
      return false;
    }
  }

  NGL_LOG(_T("nuiWidgetCreator"), NGL_LOG_WARNING, _T("nuiColor warning : could not find any color definition from '%s'\n"), rString.GetChars());

  return false;
}

void nuiColor::Get(nglString& rString) const
{
  uint32 val = 0;
  nuiColor t(*this);
  t.UnPremultiply();
  val += ToZero(t.mAlpha * 255.0f);
  val<<=8;                    
  val += ToZero(t.mRed   * 255.0f);
  val<<=8;
  val += ToZero(t.mGreen * 255.0f);
  val<<=8;
  val += ToZero(t.mBlue  * 255.0f);
  rString.SetCUInt(val,16); 
  rString.Insert('#',0);
}

nglString nuiColor::GetValue() const
{
  nglString String;
  uint32 val = 0;
  val += (uint32)(mAlpha * 255.0f);
  val<<=8;
  val += (uint32)(mRed   * 255.0f);
  val<<=8;
  val += (uint32)(mGreen * 255.0f);
  val<<=8;
  val += (uint32)(mBlue  * 255.0f);
  String.SetCUInt(val,16); 
  String.Insert('#',0);
  return String;
}

void nuiColor::EnumStandardColorNames(std::vector<nglString>& rStandardColorNames)
{
  for (int i = 0; i < 147; i++)
  {
    rStandardColorNames.push_back(static_colors[i].name);
  }
}

void nuiColor::Crop()
{
#ifdef NUI_USE_SSE
  const float* pElts = &mRed;
  const float one[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  __asm
  {
    mov eax, pElts; // eax = &mRed( pointer to the colors)
    xorps xmm1, xmm1; // xmm1 = 0
    movups xmm0, [eax]; // xmm0 = (red|green|blue|alpha)
    movups xmm2, one;

    maxps xmm0, xmm1;
    minps xmm0, xmm2;
    movups [eax], xmm0; //< Store the values back
  }
#else
  if (std::isnan(mRed))
    mRed = 0;
  if (std::isnan(mGreen))
    mGreen = 0;
  if (std::isnan(mBlue))
    mBlue = 0;
  if (std::isnan(mAlpha))
    mAlpha = 0;
  mRed   = MIN(1.0f, mRed  );
  mRed  =  MAX(0.0f, mRed  );
  mGreen = MIN(1.0f, mGreen);
  mGreen = MAX(0.0f, mGreen);
  mBlue  = MIN(1.0f, mBlue );
  mBlue  = MAX(0.0f, mBlue );
  mAlpha = MIN(1.0f, mAlpha);
  mAlpha = MAX(0.0f, mAlpha);
#endif
}

void nuiColor::EnumCustomizedColorNames(std::vector<nglString>& rUserColorNames)
{
  rUserColorNames.clear();
  rUserColorNames.reserve(mCustomColors.size());
  
  ColorMap::iterator it = mCustomColors.begin();
  ColorMap::iterator end = mCustomColors.end();
  
  while (it != end)
  {
    rUserColorNames.push_back(it->first);
    ++it;
  }
}

void nuiColor::SetColor(const nglString& rName, const nuiColor& rColor)
{
  mCustomColors[rName] = rColor;
}

bool nuiColor::GetColor(const nglString& rName, nuiColor& rColor)
{
  ColorMap::iterator it = mCustomColors.find(rName);
  if (it == mCustomColors.end())
    return false;
  
  rColor = it->second;
  return true;
}

nuiColor::ColorMap nuiColor::mCustomColors;

uint32 nuiColor::GetRGBA() const
{
  uint32 t = 0;
  uint8* pt = (uint8*)&t;
  pt[0] = ToBelow(mRed   * 255.0f);
  pt[1] = ToBelow(mGreen * 255.0f);
  pt[2] = ToBelow(mBlue  * 255.0f);
  pt[3] = ToBelow(mAlpha * 255.0f);
  return t;
}

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
void RGBtoHSV( float r, float g, float b, float& h, float& s, float& v )
{
	float _min, _max, delta;
	_min = MIN(r, MIN(g, b));
	_max = MAX(r, MAX(g, b));
	v = _max;				// v
	delta = _max - _min;
	if (_max != 0)
		s = delta / _max;		// s
	else
  {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = -1;
		return;
	}
	if (r == _max)
		h = (g - b) / delta;		// between yellow & magenta
	else if( g == _max )
		h = 2 + (b - r) / delta;	// between cyan & yellow
	else
		h = 4 + (r - g) / delta;	// between magenta & cyan
	h *= 60;				// degrees
	while (h < 0)
		h += 360;
	while (h >= 360)
		h -= 360;
  h /= 360;
  
  if (std::isnan(h))
    h = 0;
  if (std::isnan(s))
    s = 0;
  if (std::isnan(v))
    v = 0;
}

void HSVtoRGB( float& r, float& g, float& b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
  h *= 360;
  while (h >= 360)
    h -= 360;
  while (h < 0)
    h += 360;
  
	if (s == 0)
  {
		// achromatic (grey)
		r = g = b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor(h);
	f = h - i;			// factorial part of h
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i)
  {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
  
  if (std::isnan(r))
    r = 0;
  if (std::isnan(g))
    g = 0;
  if (std::isnan(b))
    b = 0;
  
}


void nuiColor::SetHSV(float h, float s, float v, float a)
{
  HSVtoRGB(mRed, mGreen, mBlue, h, s, v);
  mAlpha = a;
  Premultiply();
  Crop();
}

void nuiColor::GetHSV(float& h, float& s, float& v) const
{
  nuiColor c(*this);
  c.UnPremultiply();
  float r = c.Red(), g = c.Green(), b = c.Blue();
  RGBtoHSV(r, g, b, h, s, v);  
}

void nuiColor::SetHSL(float h, float s, float l, float a)
{
  double v;
  double r,g,b;

  r = l;   // default to gray
  g = l;
  b = l;
  v = (l <= 0.5) ? (l * (1.0 + s)) : (l + s - l * s);
  if (v > 0)
  {
    double m;
    double sv;
    int sextant;
    double fract, vsf, mid1, mid2;
    
    m = l + l - v;
    sv = (v - m ) / v;
    h *= 6.0;
    sextant = (int)h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;
    switch (sextant)
    {
      case 0:
        r = v;
        g = mid1;
        b = m;
        break;
      case 1:
        r = mid2;
        g = v;
        b = m;
        break;
      case 2:
        r = m;
        g = v;
        b = mid1;
        break;
      case 3:
        r = m;
        g = mid2;
        b = v;
        break;
      case 4:
        r = mid1;
        g = m;
        b = v;
        break;
      case 5:
        r = v;
        g = m;
        b = mid2;
        break;
    }
  }
  
  if (std::isnan(r))
    r = 0;
  if (std::isnan(g))
    g = 0;
  if (std::isnan(b))
    b = 0;
  if (std::isnan(a))
    a = 0;
  
  mRed = r;
  mGreen = g;
  mBlue = b;
  mAlpha = a;

  Premultiply();
  Crop();
}

void nuiColor::GetHSL(float& h, float& s, float& l) const
{
  nuiColor c(*this);
  c.UnPremultiply();
  double r = c.Red();
  double g = c.Green();
  double b = c.Blue();
  double v;
  double m;
  double vm;
  double r2, g2, b2;
  
  h = 0; // default to black
  s = 0;
  l = 0;
  
  v = MAX(r,g);
  v = MAX(v,b);
  m = MIN(r,g);
  m = MIN(m,b);
  l = (m + v) / 2.0;
  
  if (l <= 0.0)
    return;

  vm = v - m;
  s = vm;
  if (s > 0.0)
  {
    s /= (l <= 0.5) ? (v + m ) : (2.0 - v - m) ;
  }
  else
  {
    return;
  }
  r2 = (v - r) / vm;
  g2 = (v - g) / vm;
  b2 = (v - b) / vm;
  if (r == v)
  {
    h = (g == m ? 5.0 + b2 : 1.0 - g2);
  }
  else if (g == v)
  {
    h = (b == m ? 1.0 + r2 : 3.0 - b2);
  }
  else
  {
    h = (r == m ? 3.0 + g2 : 5.0 - r2);
  }
  h /= 6.0;

  if (std::isnan(h))
    h = 0;
  if (std::isnan(s))
    s = 0;
  if (std::isnan(l))
    l = 0;
}

void nuiColor::SetOpacity(float v)
{
  UnPremultiply();
  mAlpha = nuiClamp(v, 0.0f, 1.0f);

  Premultiply();
}

float nuiColor::GetOpacity() const
{
  nuiColor c(*this);
  c.UnPremultiply();
  return c.mAlpha;
}
