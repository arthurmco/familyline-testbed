(define button-appearance
      '((background . #(0 0 0 0.8))
        (font-size . 16)
        (max-height . 80)
        (max-width . 360)))

(define (on-main-menu-open val)
  (let ((win
         (window-create
          "menu"
          (use-layout 'flex 'vertical)
          (list (control-create "ltitle"
                                type: 'label
                                appearance: '((max-height . 45)
                                              (font . "Inconsolata")
                                              (font-size . 32)
                                              (horizontal-alignment . center)
                                              (background . #(0 0 0 0))
                                              (foreground . #(1 1 1 1)))
                                text:  "FAMILYLINE")
                (control-create "lversion"
                                type: 'label
                                appearance: '((max-height . 45)
                                              (font-size . 14)
                                              (background . #(1 1 1 0.5))
                                              (foreground . #(0.2 0.2 1 1)))
                                text:  (format #f "Version ~a (commit ~a)"
                                               (call-public 'get-version)
                                               (call-public 'get-commit-id)))
                (control-create "btnNew"
                                type: 'button
                                text: "New Game"
                                appearance: button-appearance
                                click-handler:
                                (lambda (b)
                                  (call-public 'start-game)))
                (control-create "btnMultiPlayer"
                                type: 'button
                                appearance: button-appearance
                                text: "Multiplayer"
                                click-handler:
                                (lambda (b)
                                  (let ((wmultiplayer
                                         (window-create
                                          "multiplayer"
                                          (use-layout 'flex 'vertical)
                                          (list
                                           (control-create "stitle"
                                                           type: 'label
                                                           appearance: '((max-height . 35)
                                                                         (background . #(0 0 0 1))
                                                                         (font-size . 20))
                                                           text:  "Multiplayer")
                                           (control-create
                                            ""
                                            type: 'box
                                            layout: (use-layout 'flex 'horizontal)
                                            appearance: '((max-height . 90))
                                            children: (list
                                                       (control-create "btnBack"
                                                                       type: 'button
                                                                       text: "Back"
                                                                       click-handler:
                                                                       (lambda (b)
                                                                         (window-destroy "multiplayer")
                                                                         (window-move-to-top "menu")))

                                                       (control-create "btnConnect"
                                                                       type: 'button
                                                                       text: "Connect"
                                                                       click-handler:
                                                                       (lambda (b)
                                                                         (window-destroy "multiplayer")
                                                                         (window-move-to-top "menu")))

                                                       
                                                       ))))))
                                  (window-show wmultiplayer))))
                (control-create
                 "btnSettings"
                 type: 'button
                 appearance: button-appearance
                 text: "Settings"
                 click-handler:
                 (lambda (b)
                   (let ((wsettings
                          (window-create
                           "settings"
                           (use-layout 'flex 'vertical)
                           (list
                            (control-create "stitle"
                                type: 'label
                                appearance: '((max-height . 35)
                                              (background . #(0 0 0 1))
                                              (font-size . 20))
                                text:  "Settings")
                            (control-create "lblName"
                                type: 'label
                                appearance: '((max-height . 35)
                                              (background . #(0 0 0 1)))
                                text:  "Player name")
                            (control-create "txtName"
                                type: 'textbox
                                appearance: '((max-height . 35))
                                text:  (get-config-option "player/username"))
                            (control-create
                             ""
                             type: 'box
                             appearance: '((max-height . 35))
                             layout: (use-layout 'flex 'horizontal)
                             children: (list
                                        (control-create
                                         "chkEnableRecord"
                                         type: 'checkbox
                                         appearance: '((background . #(0 0 0 1)))
                                         active: (get-config-option "enable-input-recording"))
                                        (control-create
                                         "lblEnableRecord"
                                         type: 'label
                                         appearance: '((background . #(0 0 0 1)))
                                         text:  "Enable input recording")))
                            (control-create "btnBack"
                                            type: 'button
                                            text: "Back"
                                            click-handler:
                                            (lambda (b)
                                              (set-config-option
                                               "player/username"
                                               (control-get-property (control-get "txtName")
                                                                     'text))
                                              (set-config-option
                                               "enable-input-recording"
                                               (control-get-property (control-get "chkEnableRecord")
                                                                     'active))
                                              (window-destroy "settings")
                                              (window-move-to-top "menu")))))))
                     (window-show wsettings))))
                              (control-create "btnQuit"
                                              type: 'button
                                              appearance: button-appearance
                                              text: "Exit"
                                              click-handler:
                                              (lambda (b)
                                                (call-public 'exit-game)))))))


    (window-show win)))
