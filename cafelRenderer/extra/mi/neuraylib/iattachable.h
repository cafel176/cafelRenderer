/***************************************************************************************************
 * Copyright 2018 NVIDIA Corporation. All rights reserved.
 **************************************************************************************************/
/// \file
/// \brief Type that holds a reference to a database element or a value.

#ifndef MI_NEURAYLIB_IATTACHABLE_H
#define MI_NEURAYLIB_IATTACHABLE_H

#include <mi/neuraylib/idata.h>

namespace mi {

class IAttachable :
    public base::Interface_declare<0x5e7a28b7,0x38aa,0x45ac,0x98,0x65,0x97,0xac,0xcd,0x81,0xc4,0x0e,
                                   IData_simple>
{
public:
    virtual Sint32 set_reference( const base::IInterface* db_element) = 0;

    virtual Sint32 set_reference( const char* name) = 0;

    virtual const base::IInterface* get_reference() const = 0;

    template <class T>
    const T* get_reference() const
    {
        const base::IInterface* ptr_iinterface = get_reference();
        if ( !ptr_iinterface)
            return 0;
        const T* ptr_T = static_cast<const T*>( ptr_iinterface->get_interface( typename T::IID()));
        ptr_iinterface->release();
        return ptr_T;
    }

    virtual base::IInterface* get_reference() = 0;

    template <class T>
    T* get_reference()
    {
        base::IInterface* ptr_iinterface = get_reference();
        if ( !ptr_iinterface)
            return 0;
        T* ptr_T = static_cast<T*>( ptr_iinterface->get_interface( typename T::IID()));
        ptr_iinterface->release();
        return ptr_T;
    }

    virtual const char* get_reference_name() const = 0;

    virtual const IData* get_value() const = 0;

    template<class T>
    const T* get_value() const
    {
        const IData* ptr_idata = get_value();
        if ( !ptr_idata)
            return 0;
        const T* ptr_T = static_cast<const T*>( ptr_idata->get_interface( typename T::IID()));
        ptr_idata->release();
        return ptr_T;
    }

    virtual IData* get_value() = 0;

    template<class T>
    T* get_value()
    {
        IData* ptr_idata = get_value();
        if ( !ptr_idata)
            return 0;
        T* ptr_T = static_cast<T*>( ptr_idata->get_interface( typename T::IID()));
        ptr_idata->release();
        return ptr_T;
    }
};

/*@}*/ // end group mi_neuray_simple_types

} // namespace mi

#endif // MI_NEURAYLIB_IATTACHABLE_H
