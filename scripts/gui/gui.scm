(define win
  (window-create "menu"
                 (use-layout 'flex 'vertical)
                 (list (control-create "l2"
                                       type: 'label
                                       appearance: '((max-height . 40))
                                       text:  " DENTRO DO SCHEME")
                       (control-create
                        "b" type: 'box
                        layout: (use-layout 'flex 'horizontal)
                        children: (list
                                   (control-create "l"
                                                   type: 'label
                                                   text: "Que bonito bolo, que bonitas velas")
                                   (control-create "lother"
                                                   type: 'label
                                                   text: "OLHA SEM C++!!!!")
                                   (control-create "l1"
                                                   type: 'label
                                                   appearance: '((background . #(0.2 0 0 0.1))
                                                                 (foreground . #(0 1 1 1)))
                                                   text: "Que bonito corpo, acho que ele está me esquentando")))
                       (control-create "bt"
                                       type: 'button
                                       text: "Eu tenho um presente pra você..."
                                       click-handler:
                                       (lambda (b)
                                         (display b)
                                         (set-appearance-of (control-get "l")
                                                            '((background . #(1 0 0 1))
                                                              (foreground . #(0 1 1 1))))
                                         (control-set b 'text "Vc clicou no butão")
                                         )))))




(set-appearance-of (control-get "l2")
                    '((background . #(1 1 1 1))
                      (foreground . #(1 0 0 1))))

