(define button-appearance
  '((background . #(0 0 0 0.8))
    (font-size . 16)
    (max-height . 80)
    (max-width . 360)))

(define (multiplayer-error-msg msg-keyword addr)
  (cond
   ((eq? msg-keyword 'connection-error)
    (format #f "~A was not found or the firewall is blocking it" addr))
   ((eq? msg-keyword 'wrong-password)
    (format #f "You typed the wrong password for ~A" addr))
   ((eq? msg-keyword 'login-failure)
    (format #f "~A was found, but we could not log into it" addr))
   ((eq? msg-keyword 'connection-timeout)
    (format #f "Timeout while connecting into ~A" addr))
   ((eq? msg-keyword 'server-error)
    (format #f "~A sent some data that we could not comprehend" addr))
   ((eq? msg-keyword 'not-all-clients-connected)
    (format #f "Tried to connect to ~A, but not all clients were ready " addr))
   (else (format #f "Unknown error while connecting to ~A" addr))
   )
  )

(define (multiplayer-click-handler b)
  (let* ((server-cb (multiplayer-listen-start
                     (lambda (name addr)
                       (listbox-add-item-tag
                        (control-get "lclients")
                        addr
                        (string-append "<b>'" name "'</b> &lt;" addr "&gt;" )))))
         (wmultiplayer
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
            (control-create "lclients"
                            type: 'listbox
                            on-selected-item-change:
                            (lambda (c idx tag)
                              (control-set-textbox
                               (control-get "txtAddress")
                               'text tag)))
            (control-create "txtAddress"
                            type: 'textbox
                            appearance: '((max-height . 35))
                            text: "")
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
                                          (multiplayer-listen-stop server-cb)
                                          (window-destroy "multiplayer")
                                          (window-move-to-top "menu")))

                        (control-create "btnConnect"
                                        type: 'button
                                        text: "Connect"
                                        click-handler:
                                        (lambda (b)
                                          (multiplayer-listen-stop server-cb)
                                          (let ((connectres (multiplayer-connect
                                                              (control-get-property
                                                               (control-get "txtAddress") 'text))))
                                                (display connectres)
                                                (if (multiplayer-connected? connectres)
                                                    (begin
                                                      (window-destroy "multiplayer")
                                                      (window-move-to-top "menu")
                                                      (window-destroy "menu")
                                                      (window-destroy "bg")
                                                      (multiplayer-login connectres)
                                                      (on-main-menu-open #f))
                                                    (let ((errmsg (multiplayer-get-error connectres))
                                                          (addr (multiplayer-get-login-address
                                                                 connectres)))
                                                      (show-message-box
                                                       (string-append "Error: "
                                                                      (symbol->string errmsg))
                                                       (multiplayer-error-msg errmsg addr)))))))))))))
    (window-show wmultiplayer)))

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
                                click-handler: multiplayer-click-handler)
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
