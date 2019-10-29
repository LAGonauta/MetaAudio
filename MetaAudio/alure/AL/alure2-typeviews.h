/*********
 * Implements the classes alure::ArrayView (non-owning, read-only access to a
 * contiguous array of elements), and alure::StringView (non-owning, read-only
 * access to an array of chars). These help pass around contiguous arrays and
 * strings from various sources without copying.
 */

#ifndef AL_ALURE2_TYPEVIEWS_H
#define AL_ALURE2_TYPEVIEWS_H

#include <stdexcept>
#include <iostream>
#include <cstring>

#include "alure2-aliases.h"

namespace alure {

// Tag specific containers that guarantee contiguous storage. The standard
// provides no such mechanism, so we have to manually specify which are
// acceptable.
template<typename T> struct IsContiguousTag : std::false_type {};
template<typename T, size_t N> struct IsContiguousTag<Array<T,N>> : std::true_type {};
template<typename... Args> struct IsContiguousTag<Vector<Args...>> : std::true_type {};
template<typename... Args> struct IsContiguousTag<BasicString<Args...>> : std::true_type {};

// A rather simple ArrayView container. This allows accepting various array
// types (Array, Vector, a static-sized array, a dynamic array + size) without
// copying its elements.
template<typename T>
class ArrayView {
public:
    using value_type = T;

    using iterator = const value_type*;
    using const_iterator = const value_type*;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using size_type = size_t;

    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    const value_type *mStart;
    const value_type *mEnd;

public:
    ArrayView() noexcept : mStart(nullptr), mEnd(nullptr) { }
    ArrayView(const ArrayView&) noexcept = default;
    ArrayView(ArrayView&&) noexcept = default;
    ArrayView(const value_type *elems, size_type num_elems) noexcept
      : mStart(elems), mEnd(elems+num_elems) { }
    // TODO: Allow this? As a function parameter, making a view to a temporary
    // is fine since the temporary will exist for the duration of the call.
    // It's just a problem when done as a local variable.
    template<typename OtherT> ArrayView(RemoveRefT<OtherT>&&) = delete;
    template<typename OtherT,
             typename = EnableIfT<IsContiguousTag<RemoveRefT<OtherT>>::value>>
    ArrayView(const OtherT &rhs) noexcept : mStart(rhs.data()), mEnd(rhs.data()+rhs.size()) { }
    template<size_t N>
    ArrayView(const value_type (&elems)[N]) noexcept : mStart(elems), mEnd(elems+N) { }

    ArrayView& operator=(const ArrayView&) noexcept = default;

    const value_type *data() const noexcept { return mStart; }

    size_type size() const noexcept { return mEnd - mStart; }
    bool empty() const noexcept { return mStart == mEnd; }

    const value_type& operator[](size_t i) const noexcept { return mStart[i]; }

    const value_type& front() const noexcept { return *mStart; }
    const value_type& back() const noexcept { return *(mEnd - 1); }

    const value_type& at(size_t i) const
    {
        if(i >= size())
            throw std::out_of_range("alure::ArrayView::at: element out of range");
        return mStart[i];
    }

    const_iterator begin() const noexcept { return mStart; }
    const_iterator cbegin() const noexcept { return mStart; }

    const_iterator end() const noexcept { return mEnd; }
    const_iterator cend() const noexcept { return mEnd; }

    const_reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    const_reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    ArrayView slice(size_type pos, size_type len = npos) const
    {
        if(pos > size())
            throw std::out_of_range("alure::ArrayView::slice: pos out of range");
        if(size()-pos < len) return ArrayView(data()+pos, size()-pos);
        return ArrayView(data()+pos, len);
    }

    template<typename U>
    ArrayView<U> reinterpret_as() const
    {
        // Make sure the current view is properly aligned to be interpreted as
        // the new type.
        if((reinterpret_cast<std::intptr_t>(mStart) & (alignof(U)-1)) != 0)
            throw std::runtime_error(
                "alure::ArrayView::reinterpret_as: invalid alignment for type");

        size_t new_length =
            (reinterpret_cast<const char*>(mEnd) - reinterpret_cast<const char*>(mStart)) /
            sizeof(U);
        return ArrayView<U>(reinterpret_cast<const U*>(mStart), new_length);
    }
};

template<typename T, typename Tr=std::char_traits<T>>
class BasicStringView : public ArrayView<T> {
public:
    using char_type = T;
    using traits_type = Tr;
    using size_type = size_t;

    static constexpr size_type npos = static_cast<size_type>(-1);

    BasicStringView() noexcept = default;
    BasicStringView(const BasicStringView&) noexcept = default;
    BasicStringView(const char_type *elems, size_type num_elems) noexcept
      : ArrayView<T>(elems, num_elems) { }
    BasicStringView(const char_type *elems) : ArrayView<T>(elems, traits_type::length(elems)) { }
    template<typename Alloc>
    BasicStringView(BasicString<T,Tr,Alloc>&&) = delete;
    template<typename Alloc>
    BasicStringView(const BasicString<T,Tr,Alloc> &rhs) noexcept : ArrayView<T>(rhs) { }
#if __cplusplus >= 201703L
    BasicStringView(const std::basic_string_view<T,Tr> &rhs) noexcept
      : ArrayView<T>(rhs.data(), rhs.length()) { }
#endif

    BasicStringView& operator=(const BasicStringView&) noexcept = default;

    size_type length() const noexcept { return this->size(); }

    template<typename Alloc>
    explicit operator BasicString<T,Tr,Alloc>() const
    { return BasicString<T,Tr,Alloc>(this->data(), length()); }
#if __cplusplus >= 201703L
    operator std::basic_string_view<T,Tr>() const noexcept
    { return std::basic_string_view<T,Tr>(this->data(), length()); }
#endif

    template<typename Alloc>
    BasicString<T,Tr,Alloc> operator+(const BasicString<T,Tr,Alloc> &rhs) const
    {
        BasicString<T,Tr,Alloc> ret = BasicString<T,Tr,Alloc>(*this);
        ret += rhs;
        return ret;
    }

    int compare(BasicStringView other) const noexcept
    {
        int ret = traits_type::compare(
            this->data(), other.data(), std::min<size_t>(length(), other.length())
        );
        if(ret == 0)
        {
            if(length() > other.length()) return 1;
            if(length() < other.length()) return -1;
            return 0;
        }
        return ret;
    }
    bool operator==(BasicStringView rhs) const noexcept { return compare(rhs) == 0; }
    bool operator!=(BasicStringView rhs) const noexcept { return compare(rhs) != 0; }
    bool operator<=(BasicStringView rhs) const noexcept { return compare(rhs) <= 0; }
    bool operator>=(BasicStringView rhs) const noexcept { return compare(rhs) >= 0; }
    bool operator<(BasicStringView rhs) const noexcept { return compare(rhs) < 0; }
    bool operator>(BasicStringView rhs) const noexcept { return compare(rhs) > 0; }

    BasicStringView substr(size_type pos, size_type len = npos) const
    {
        if(pos > length())
            throw std::out_of_range("alure::BasicStringView::substr: pos out of range");
        if(length()-pos < len) return BasicStringView(this->data()+pos, length()-pos);
        return BasicStringView(this->data()+pos, len);
    }

    size_type find_first_of(char_type ch, size_type pos = 0) const noexcept
    {
        if(pos >= length()) return npos;
        const char_type *chpos = traits_type::find(this->data()+pos, length()-pos, ch);
        if(chpos) return chpos - this->data();
        return npos;
    }
    size_type find_first_of(BasicStringView other, size_type pos = 0) const noexcept
    {
        size_type ret = npos;
        for(auto ch : other)
            ret = std::min<size_type>(ret, find_first_of(ch, pos));
        return ret;
    }
};
using StringView = BasicStringView<String::value_type>;

// Inline operators to concat Strings with StringViews.
template<typename T, typename Tr, typename Alloc>
inline BasicString<T,Tr,Alloc> operator+(const BasicString<T,Tr,Alloc> &lhs, BasicStringView<T,Tr> rhs)
{ return BasicString<T,Tr,Alloc>(lhs).append(rhs.data(), rhs.size()); }
template<typename T, typename Tr, typename Alloc>
inline BasicString<T,Tr,Alloc> operator+(BasicString<T,Tr,Alloc>&& lhs, BasicStringView<T,Tr> rhs)
{ return std::move(lhs.append(rhs.data(), rhs.size())); }
template<typename T, typename Tr, typename Alloc>
inline BasicString<T,Tr,Alloc>& operator+=(BasicString<T,Tr,Alloc> &lhs, BasicStringView<T,Tr> rhs)
{ return lhs.append(rhs.data(), rhs.size()); }

// Inline operators to compare String and C-style strings with StringViews.
#define ALURE_DECL_STROP(op)                                                     \
template<typename T, typename Tr, typename Alloc>                                \
inline bool operator op(const BasicString<T,Tr,Alloc> &lhs,                      \
                        BasicStringView<T,Tr> rhs) noexcept                      \
{ return BasicStringView<T,Tr>(lhs) op rhs; }                                    \
template<typename T, typename Tr>                                                \
inline bool operator op(const typename BasicStringView<T,Tr>::value_type *lhs,   \
                        BasicStringView<T,Tr> rhs)                               \
{ return BasicStringView<T,Tr>(lhs) op rhs; }
ALURE_DECL_STROP(==)
ALURE_DECL_STROP(!=)
ALURE_DECL_STROP(<=)
ALURE_DECL_STROP(>=)
ALURE_DECL_STROP(<)
ALURE_DECL_STROP(>)
#undef ALURE_DECL_STROP

// Inline operator to write out a StringView to an ostream
template<typename T, typename Tr>
inline std::basic_ostream<T,Tr>& operator<<(std::basic_ostream<T,Tr> &lhs, BasicStringView<T,Tr> rhs)
{
    for(auto ch : rhs)
        lhs << ch;
    return lhs;
}

} // namespace alure

#endif /* AL_ALURE2_TYPEVIEWS_H */
