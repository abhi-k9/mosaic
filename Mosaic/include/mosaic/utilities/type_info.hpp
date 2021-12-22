#pragma once

/*! @file type_info.hpp
 *  @brief Provides `TypeInfo` wrapper class around `type_info`.
 *  @details It provides:
 *      - All the member functions of `type_info`.    
 *      - Value semantics (copy constructor and assignment operator).
 *      - Seamless comparisons through `operator<` and `operator==`.
 */

#include <typeinfo>

namespace mosaic {

class TypeInfo
{
public:

    TypeInfo() = default;  // Needed for containers.
    TypeInfo(const std::type_info& t_info);
    TypeInfo(const TypeInfo&);
    TypeInfo& operator=(const TypeInfo&);

    bool before(const TypeInfo&) const;
    const char* name() const;

private:

    const std::type_info *pInfo_ = nullptr;


    friend bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
    friend bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);
    friend bool operator<(const TypeInfo& lhs, const TypeInfo& rhs);
    friend bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs);
    friend bool operator>(const TypeInfo& lhs, const TypeInfo& rhs);
    friend bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs);

};


bool operator==(const TypeInfo& lhs, const TypeInfo& rhs);
bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs);
bool operator<(const TypeInfo& lhs, const TypeInfo& rhs);
bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs);
bool operator>(const TypeInfo& lhs, const TypeInfo& rhs);
bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs);


} // end namespace mosaic