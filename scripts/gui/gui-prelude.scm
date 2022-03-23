;;; Scheme-C glue code for defining interfaces in scheme
;;;
;;; Defining interface code in a script language is good, because
;;; we do not need to recompile, and the client does not need
;;; to know how to use a compiler.

;;; We need support for some SRFIs, though. s7 implements them already, but
;;; I will let them here just in case
;;; (use-modules (srfi srfi-88)) ; keywords
;;; (use-modules (srfi srfi-89)) ; keyword and optional args

(define (use-layout type param)
  "Create a representation of a layout."
  "(Because the layouts are templated, we cannot instantiate them here, so, FOR NOW, we"
  "will create an ephemeral object and pass there"
  (cons type param))

(define* (window-create name layout (children #f))
  "Create a window on the current GUI manager for this context."
  (let ((w (current-manager-add-window name layout)))
    (when (list? children)
      (newline)
      (map (lambda (c) (window-add-control w c)) children)
      (map display children))
    w))


(define (control-set control property value)
  "Set a certain property of a control"
  ;; We determine the control type, then forward it to the appropriate
  ;; function into the native code
  (let ((ctype (car control)))
    (cond
     ((eq? ctype #:textbox) (control-set-textbox control property value))
     ((eq? ctype #:button) (control-set-button control property value))
     ((eq? ctype #:listbox) (control-set-listbox control property value))
     (else (error "Invalid control type " ctype)))))

(define (control-get-property control property) 
  "Get a certain property of a control"
  ;; We determine the control type, then forward it to the appropriate
  ;; function into the native code
  (let ((ctype (car control)))
    (cond
     ((eq? ctype #:textbox) (control-get-textbox-property control property))
     ((eq? ctype #:checkbox) (control-get-checkbox-property control property))
     (else (error "Invalid control type " ctype)))))

(define (listbox-add-item-tag listbox tag item)
  (control-set-listbox listbox 'add-item (cons tag item)))

(define (listbox-add-item listbox item)
  (listbox-add-item-tag listbox item item))


(define (listbox-set-item listbox tag item)
  (control-set-listbox listbox 'set-item (cons tag item)))

(define (listbox-remove-item listbox tag)
  (control-set-listbox listbox 'remove-item tag))


(define* (control-create name type appearance
                         ;; more or less common
                         text
                         ;; box
                         layout children
                         ;; checkbox
                         active
                         ;; listbox
                         items on-selected-item-change
                         ;; button
                         click-handler)
  "Create a control"
  "Depending on the control type, forward the function to the specific C++ control"
  "creation function"
  (let ((control (cond
                  ((eq? type 'box) (control-create-box name layout children))
                  ((eq? type 'label) (control-create-label name text))
                  ((eq? type 'button) (control-create-button name text click-handler))
                  ((eq? type 'textbox) (control-create-textbox name text))
                  ((eq? type 'checkbox) (control-create-checkbox name active))
                  ((eq? type 'listbox)
                   (let ((c (control-create-listbox name items)))
                     (if (procedure? on-selected-item-change)
                         (control-set-listbox
                          c
                          'on-selected-item-change
                          on-selected-item-change))
                     c))
                  (else (error "Invalid control type " type)))))
    (when (list? appearance)
      (set-appearance-of control appearance))
    control))



(define (get-config-option name)
  (call-public 'get-config-option name))


(define (set-config-option name value)
  (call-public 'set-config-option (cons name value)))


(define server-callback (lambda (name addr) (display (string-append name " " addr))))
(define (multiplayer-listen-start callback)
  "Start listening for multiplayer servers."
  "When a server appears, `callback` will be called, with the server name and"
  "address"
  "Returns an ID, so you can stop the callback later"
  (set! server-callback callback)
  (display server-callback)
  1)


(define (show-message-box title msg)
  "Show a message box, with the text 'name' and the title 'title' on it"
  (call-public 'show-message-box '(title msg)))


(define (multiplayer-listen-stop handle)
  (set! server-callback (lambda (name addr) #f)))


(define (multiplayer-connect address)
  (call-public 'multiplayer-connect address))
