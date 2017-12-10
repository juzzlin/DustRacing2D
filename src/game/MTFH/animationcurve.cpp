#include "animationcurve.hpp"

#include <cassert>
#include <cmath>

namespace MTFH {

AnimationCurve::AnimationCurve(int steps, int exp)
    : m_values(steps, 1)
{
    assert(steps > 2);
    assert(exp >= 1);

    float index = 0;
    for (float & value : m_values)
    {
        value = std::pow(index / (steps - 1), exp);
        index++;
    }
}

void AnimationCurve::step()
{
    if (m_index + 1 < static_cast<int>(m_values.size()))
    {
        m_index++;
    }
}

void AnimationCurve::reset()
{
    m_index = 0;
}

float AnimationCurve::value() const
{
    return m_values.at(m_index);
}

}
