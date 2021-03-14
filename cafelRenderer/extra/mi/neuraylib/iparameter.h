/***************************************************************************************************
 * Copyright 2018 NVIDIA Corporation. All rights reserved.
 **************************************************************************************************/
/// \file
/// \brief Numeric types.

#ifndef MI_NEURAYLIB_IPARAMETER_H
#define MI_NEURAYLIB_IPARAMETER_H

#include <mi/neuraylib/idata.h>

namespace mi {

class IParameter :
    public base::Interface_declare<0xbdef76e3,0x465e,0x471c,0xb9,0x70,0x5f,0x2f,0x2d,0xad,0x35,0xd4,
                                   IData_simple>
{
public:
    virtual void get_value( Uint32& val) const = 0;

    virtual void set_value( Uint32 val) = 0;

    Uint32 get_value() const { Uint32 value; get_value( value); return value; }
};

} // namespace mi

#endif // MI_NEURAYLIB_IPARAMETER_H
