#include "MathLib.h"
#include <array>
#include <cmath>

static std::array<float, 256> normCircleTable;

void setupLookupTables()
{
    // Lookup table for normCircleFcn
    const int numElems = normCircleTable.size();
    for(int i=0 ; i<numElems ; i++)
    {
        float fx = float(i)/float(numElems);
        normCircleTable[i] = sqrt(1.0f - fx*fx);
    }
}

float normCircleFcn(const float a)
{
    return normCircleTable[int(a*float(normCircleTable.size()))];
}
