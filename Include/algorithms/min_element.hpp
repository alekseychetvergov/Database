#pragma once

#include <functional>
#include <iterator>

template <std::forward_iterator Iterator, typename Compare = std::less<>>
constexpr Iterator minElement(Iterator first, Iterator last, Compare compare = {})
{
    if (first == last)
        return last;

    auto minimum = first;
    for (++first; first != last; ++first)
    {
        if (std::invoke(compare, *first, *minimum))
            minimum = first;
    }
    return minimum;
}