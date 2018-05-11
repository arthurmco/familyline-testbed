;;;
;;; Familyline terrain editing mode for Emacs, because vim is crap
;;; Copyright(C) 2017 Arthur M
;;;
;;; Licensed under MIT License. Suck my balls, Stallman
;;;

;;; Code:
(defun string-byte-to-number (bstr nlen)
  "Convert 'BSTR', a stringified byte part with length 'NLEN' in a numeric representation."
  (let ((vval 0))
    (let ((vlist ()))
      (setq vlist (append bstr vlist))
      (dotimes (i nlen vval)
	(let ((bshift (* i 8)))
	  (setq vval (logior vval (lsh (nth i vlist) bshift))))))))


(defun tribalia-terrain/return-thdr-offset ()
  "Return the offset to the terrain header, zero-indexed."
  (when (not (string= "TRTB" (buffer-substring 1 5)))
    (error "Invalid magic number"))
  (when (/= (string-byte-to-number (buffer-substring 5 9) 4) 1)
    (error "Invalid terrain version"))
  (let (thdr-offset)
    (save-current-buffer
      (set-buffer-multibyte nil)
    (setq thdr-offset (string-byte-to-number (buffer-substring 13 17) 4)))
    (when (< thdr-offset 16)
      (error "Impossible value for terrain header offset"))
    thdr-offset))


(defun tribalia-terrain/get-thdr-info (offset)
  "Return information about the terrain that lies in the terrain header offset
Since multiple headers may lie into a single file, and there's no default 
place for terrain headers to lie, you need to specify the thdr_offset in 
the OFFSET arg."
  (let ((width 0)
	(height 0)
	(data-off 0)
	(next-thdr-off 0))
					; buffer-substring, indexes start as 1 you fuck
    (setq offset (1+ offset))
    (let ((width-off (+ offset 0))
	  (height-off (+ offset 4))
	  (next-thdr-off-off (+ offset 16))
	  (data-off-off (+ offset 20)))
      (save-current-buffer
	(set-buffer-multibyte nil)
	  (setq width (string-byte-to-number
		       (buffer-substring width-off (+ width-off 4)) 4))
	  (setq height (string-byte-to-number
			(buffer-substring height-off (+ height-off 4)) 4))
	  (setq data-off (string-byte-to-number
			  (buffer-substring data-off-off (+ data-off-off 4)) 4))
	  (setq next-thdr-off (string-byte-to-number
			       (buffer-substring next-thdr-off-off
						 (+ next-thdr-off-off 4)) 4)))
	  (let ((vret))
	    (setq vret '((width . width)
			 (height . height)
			 (data-off . data-off)
			 (next-thdr-off . next-thdr-off)))
	    (setf (alist-get 'width vret) width
		  (alist-get 'height vret) height
		  (alist-get 'data-off vret) data-off
		  (alist-get 'next-thdr-off vret) next-thdr-off)
	    vret))))

(defun tribalia-terrain-info ()
  "Show information about Familyline terrains."
  (interactive)
  (let (thdr)
    (setq thdr (tribalia-terrain/return-thdr-offset))
    (let (thdr-info)
      (setq thdr-info (tribalia-terrain/get-thdr-info thdr))
      (generate-new-buffer "Familyline terrain")
      (display-buffer "Familyline terrain")
      (with-current-buffer "Familyline terrain"
	(read-only-mode -1)
	(erase-buffer)
	(insert "Terrain Information:\n\n")
	(insert (format "Width: %d\t\tHeight: %d\n"
			(alist-get 'width thdr-info)
			(alist-get 'height thdr-info)))
	(insert (format "Data offset: %#x"
			(alist-get 'data-off thdr-info)))
	(read-only-mode)))))

(provide 'tribalia-terrain-mode)
;;; tribalia-terrain-mode.el ends here
