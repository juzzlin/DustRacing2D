#ifndef LAPCOUNTMENU_HPP
#define LAPCOUNTMENU_HPP

#include "surfacemenu.hpp"

#include <MCTextureFont>

class LapCountMenu : public SurfaceMenu
{
public:

    static std::string MenuId;

    LapCountMenu(int width, int height);

    virtual void render() override;

private:

    MCTextureFont m_font;
};

#endif // LAPCOUNTMENU_HPP
