#include <gtest/gtest.h>

#include <client/game.hpp>
#include <client/input/command_table.hpp>
#include <client/input/input_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <string>

#include "client/graphical/gfx_service.hpp"
#include "utils.hpp"
#include "utils/test_device.hpp"
#include "utils/test_framebuffer.hpp"
#include "utils/test_gui_manager.hpp"
#include "utils/test_inputprocessor.hpp"
#include "utils/test_renderer.hpp"
#include "utils/test_window.hpp"

using namespace familyline::logic;
using namespace familyline::graphics;
using namespace familyline::input;
using namespace familyline;



TEST(HumanPlayerTest, TestIfCommandTableIsOk)
{
    CommandTable ct;
    ct.loadConfiguration({
        {"w", "CameraMove, up"},
        {"s", "CameraMove, down"},
        {"<right>", "CameraMove, right"},
        {"<left>", "CameraMove, left"},
    });

    ASSERT_EQ(4, ct.size());
    
    auto upcmd = HumanInputAction{
        .timestamp = 0,
        .type      = KeyAction{
            .keycode    = SDLK_w,
            .keyname    = "",
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = 0}};
    ASSERT_TRUE(ct.actionToCommand(upcmd).has_value());
    ASSERT_EQ(
        std::tuple(PlayerCommandType::CameraMove, "up"), ct.actionToCommand(upcmd).value());

    auto rightcmd = HumanInputAction{
        .timestamp = 0,
        .type      = KeyAction{
            .keycode    = SDLK_RIGHT,
            .keyname    = "",
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = 0}};
    ASSERT_TRUE(ct.actionToCommand(rightcmd).has_value());
    ASSERT_EQ(
        std::tuple(PlayerCommandType::CameraMove, "right"), ct.actionToCommand(rightcmd).value());
}

TEST(HumanPlayerTest, TestIfCommandTableCallsUnknownKey)
{
    CommandTable ct;
    ct.loadConfiguration({
        {"w", "CameraMove, up"},
        {"<myegg>", "CameraMove, down"},
        {"<right>", "CameraMove, right"},
        {"<left>", "CameraMove, left"},
    });

    ASSERT_EQ(3, ct.size());
    
    auto upcmd = HumanInputAction{
        .timestamp = 0,
        .type      = KeyAction{
            .keycode    = SDLK_w,
            .keyname    = "",
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = 0}};
    ASSERT_TRUE(ct.actionToCommand(upcmd).has_value());
    ASSERT_EQ(
        std::tuple(PlayerCommandType::CameraMove, "up"), ct.actionToCommand(upcmd).value());

    auto rightcmd = HumanInputAction{
        .timestamp = 0,
        .type      = KeyAction{
            .keycode    = SDLK_DOWN,
            .keyname    = "",
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = 0}};
    ASSERT_FALSE(ct.actionToCommand(rightcmd).has_value());
}

TEST(HumanPlayerTest, TestIfMouseInputWorks)
{
    CommandTable ct;
    ct.loadConfiguration({
        {"w", "CameraMove, up"},
        {"C-<mouse-2>", "CameraMove, down"},
        {"<right>", "CameraMove, right"},
        {"<left>", "CameraMove, left"},
    });

    ASSERT_EQ(4, ct.size());
    
    auto upcmd = HumanInputAction{
        .timestamp = 0,
        .type      = KeyAction{
            .keycode    = SDLK_w,
            .keyname    = "",
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = 0}};
    ASSERT_TRUE(ct.actionToCommand(upcmd).has_value());
    ASSERT_EQ(
        std::tuple(PlayerCommandType::CameraMove, "up"), ct.actionToCommand(upcmd).value());

    auto rightcmd = HumanInputAction{
        .timestamp = 0,
        .type      = ClickAction{
            .screenX    = 0,
            .screenY    = 0,
            .buttonCode  = SDL_BUTTON_RIGHT,
            .clickCount = 1,
            .isPressed = true,
            .keyModifiers  = KMOD_CTRL}};
    ASSERT_TRUE(ct.actionToCommand(rightcmd).has_value());
    ASSERT_EQ(
        std::tuple(PlayerCommandType::CameraMove, "down"), ct.actionToCommand(rightcmd).value());
}
