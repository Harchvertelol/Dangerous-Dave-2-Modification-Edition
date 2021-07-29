#ifndef KEYSSTATE_H
#define KEYSSTATE_H

struct KeysState
{
    bool s_KeyLeft = false;
    bool s_KeyRight = false;
    bool s_KeyUp = false;
    bool s_KeyDown = false;
    bool s_KeyLeftUp = false;
    bool s_KeyRightUp = false;
    bool s_KeyLeftDown = false;
    bool s_KeyRightDown = false;
    bool s_KeyShoot = false;
    bool s_KeyJump = false;
};

#endif
