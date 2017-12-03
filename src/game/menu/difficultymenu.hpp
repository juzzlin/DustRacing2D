#ifndef DIFFICULTYMENU_HPP
#define DIFFICULTYMENU_HPP

#include "surfacemenu.hpp"

class DifficultyMenu : public SurfaceMenu
{
public:

    static std::string MenuId;

    DifficultyMenu(int width, int height);
};

#endif // DIFFICULTYMENU_HPP
