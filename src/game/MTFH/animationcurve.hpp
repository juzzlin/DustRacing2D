#ifndef ANIMATIONCURVE_HPP
#define ANIMATIONCURVE_HPP

#include <vector>

namespace MTFH {

class AnimationCurve
{
public:

    AnimationCurve(int steps, int exp = 2);

    virtual void step();

    virtual void reset();

    virtual float value() const;

private:

    std::vector<float> m_values;

    int m_index = 0;
};

}

#endif // ANIMATIONCURVE_HPP
