/*! @file type_info.cpp
 *  @brief Implementation for `TypeInfo` wrapper class.
 */

#include <cassert>
#include "mosaic/utilities/type_info.hpp"

namespace mosaic {

TypeInfo::TypeInfo(const std::type_info &t_info) : pInfo_(&t_info)
{
}

TypeInfo::TypeInfo(const TypeInfo &t_info) : pInfo_(t_info.pInfo_)
{
}

TypeInfo &TypeInfo::operator=(const TypeInfo &t_info)
{
    pInfo_ = t_info.pInfo_;
    return *this;
}

bool TypeInfo::before(const TypeInfo &t_info) const
{
    return pInfo_->before(*(t_info.pInfo_));
}

const char* TypeInfo::name() const
{
    return pInfo_->name();
}


bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return *(lhs.pInfo_) == *(rhs.pInfo_);
}

bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return lhs.before(rhs);
}

bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return  rhs.before(lhs);
}

bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs > rhs);
}


bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs < rhs);
}

} // end namespace mosaic