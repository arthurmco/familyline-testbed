;;; Scheme-C glue code for defining interfaces in scheme
;;;
;;; Defining interface code in a script language is good, because
;;; we do not need to recompile, and the client does not need
;;; to know how to use a compiler.

;;; We need support for some SRFIs, though
(use-modules (srfi srfi-88)) ; keywords

; guile more or less implement this automatically.
; (use-modules (srfi srfi-89)) ; keyword and optional args

(define (use-layout type param)
  "Create a representation of a layout.
   (Because the layouts are templated, we cannot instantiate them here, so, FOR NOW, we
    will create an ephemeral object and pass there"
  (cons type param))


(define* (window-create name layout #:optional children)
  "Create a window on the current GUI manager for this context."
  (display "window")
  (display name)
  (display layout)
  (let ((w (current-manager-add-window name layout)))
    (when (list? children)
      (newline)
      (display "hi")
      (map (lambda (c) (window-add-control w c)) children)
      (map display children))
    w))


(define (control-set control property value)
  "Set a certain property of a control"
  ;; We determine the control type, then forward it to the appropriate
  ;; function into the native code
  (let ((ctype (car control)))
    (cond
     [(eq? ctype #:button) (control-set-button control property value)]
     [else (error "Invalid control type " ctype)])))

(define* (control-create name #:key type appearance
                         ;; more or less common
                         text
                         ;; box
                         layout children
                         ;; button
                         click-handler)
  "Create a control
   Depending on the control type, forward the function to the specific C++ control
   creation function"
  (let ((control (cond
                  [(eq? type 'box) (control-create-box name layout children)]
                  [(eq? type 'label) (control-create-label name text)]
                  [(eq? type 'button) (control-create-button name text click-handler)]
                  [else (error "Invalid control type " type)])))
    (when (list? appearance)
      (set-appearance-of control appearance))
    control))

