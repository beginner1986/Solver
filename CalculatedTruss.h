#ifndef __CALCULATEDTRUSS_H__
#define __CALCULATEDTRUSS_H__

#include "Truss.h"

class CalculatedTruss : public Truss
{
public:
    CalculatedTruss(const Truss *truss);
};

#endif // __CALCULATEDTRUSS_H__
