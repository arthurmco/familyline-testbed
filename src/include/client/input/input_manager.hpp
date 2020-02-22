#pragma once

/**
 * Manages the user input
 *
 * (C) 2020 Arthur Mendes
 */

#include "input_processor.hpp"

#include <functional>
#include <unordered_map>
#include <queue>

namespace familyline::input {

    using HumanListenerHandler = std::function<bool(HumanInputAction)>;
    using PlayerListenerHandler = std::function<bool(PlayerInputAction)>;

    using listener_handler_t = int;

    /**
     * Manages user input, and forward it to the listeners via a
     * callback
     */
    class InputManager {
    private:
        InputProcessor& _ip;

        unsigned _tick = 0;

        std::unordered_map<listener_handler_t, HumanListenerHandler> _human_input_fns;
        std::unordered_map<listener_handler_t, PlayerListenerHandler> _player_input_fns;

        std::queue<PlayerInputAction> _input_actions;

    public:
        InputManager(InputProcessor& ip);


        /**
         * Process input events, human inputs and player inputs
         *
         * The human inputs come from the input processor
         * The player input come from the player who call `queuePlayerInput`
         */
        void processEvents();


        /**
         * Enqueue a player input
         */
        void queuePlayerInput(PlayerInputType type, int playerCode);

        /**
         * Adds a listener handler, returns an ID
         */
        listener_handler_t addListenerHandler(HumanListenerHandler h);
        listener_handler_t addListenerHandler(PlayerListenerHandler h);


        void removeListenerHandler(listener_handler_t);



    };



}  // familyline::input
