#pragma once

#include "OnMapInt.h"
#include "LiquidHolder.h"

class CTurf: public IOnMapItem
{
public:
    DECLARE_GET_TYPE_ITEM(cturf)
    DECLARE_SAVED(CTurf, IOnMapItem);
    CTurf(){};
    
};
ADD_TO_TYPELIST(CTurf);

class CGround: public CTurf
{
public:
    DECLARE_GET_TYPE_ITEM(ground);
    DECLARE_SAVED(CGround, CTurf);
    CGround();
};
ADD_TO_TYPELIST(CGround);

class Pit: public CTurf
{
public:
    Pit();
    DECLARE_SAVED(Pit, CTurf);
    DECLARE_GET_TYPE_ITEM(pit)
    virtual void process();
};
ADD_TO_TYPELIST(Pit);