/*
    Cursor information class

    Copyright (C) 2016 Arthur M
*/

#include "InputListener.hpp"
#include "InputManager.hpp"

#ifndef CURSOR_HPP
#define CURSOR_HPP

namespace familyline::input {

    class Cursor {
    private:
        static Cursor* cursor;

        InputListener* _listener;
        int _x, _y;
    public:
        static Cursor* GetInstance() {
            if (!cursor) cursor = new Cursor{};

            return cursor;
        }

        Cursor();
        void GetPositions(int& x, int& y);
    };

}

#endif /* end of include guard: CURSOR_HPP */
