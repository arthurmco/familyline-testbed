;;; Generate a global table of contents for an .org project and
;;; paste them to each resulting HTML file
;;;
;;; Very useful for generating that documentation page people
;;; generally like, and even for location
;;;

(defun get-title-info-for-current-buffer (path buffer)
    "Gets org-mode title information on the current buffer, usually
a temporary.

Advances the buffer position to after the title entry

Returns an entry in the items table '(title path level)"
    (with-current-buffer buffer
      (let* ((title-pos (re-search-forward "^#\\+TITLE:"))
             (title-end (search-forward "\n"))
             (title (string-trim (buffer-substring title-pos title-end))))
        (list title path 1 ))))

(defun get-next-section-for-current-buffer (path buffer)
    "Gets the next section for the current buffer
Returns an entry in the items table '(title path level)"
    (with-current-buffer buffer
      (let ((section-pos (re-search-forward "^\\*" nil t)))
        (if section-pos
            (let*
                ((section-end (search-forward " "))
                 (section-count (+ (length (buffer-substring section-pos section-end)) 1))
                 (title-end (search-forward "\n"))
                 (title (string-trim (buffer-substring section-end title-end))))
              (list title (concat path "::*" title) section-count ))
          nil))))

(defun get-headers-for-file (path)
  (with-temp-buffer
    (insert-file-contents path)
    (let ((items (list (get-title-info-for-current-buffer path (current-buffer))))
          (last-section (get-next-section-for-current-buffer path (current-buffer))))
      (while last-section
        (setq items (append items (list last-section)))
        (setq last-section (get-next-section-for-current-buffer path (current-buffer))))
      items)))

(setq cur-item '("Test things" "./test.org" 1))
(caddr cur-item)


(defun indent-list (level)
  (let ((indent " "))
    (dotimes (i level)
      (setq indent (concat indent "  ")))
    (concat indent " - ")
    ))

(defun accumulate-org-indices (accum item)
  (let ((name (car item))
        (link (cadr item))
        (level (caddr item)))
    (concat accum (indent-list level)
            "[[file:" link "][" name "]]\n")))


(defun create-global-toc-file-content (plist excludelist)
  "Creates the content of a global table of contents, containing the headers of 
every single org file in the project"
(seq-reduce
 'accumulate-org-indices
 (apply #'append
        (mapcar 'get-headers-for-file
                (seq-filter
                 (lambda (f) (not (-contains? excludelist f)))
                 (directory-files-recursively (plist-get plist :base-directory) ".*org$"))))
 ""))


(defun create-global-toc-file (plist excludelist)
  "Creates a global table of contents
   
   'excludelist' lists some files that you do not want to have in your
global table of contents"
  (with-temp-file "global-toc.org"
    (insert "
#+options: html-link-use-abs-url:nil html-postamble:auto html-preamble:t\n
#+options: html-scripts:nil html-style:nil html5-fancy:t tex:t\n
#+html_doctype: xhtml-strict\n
#+html_container: section\n
#+description:n
#+keywords:\n
#+html_link_home:\n
#+html_link_up:\n
#+html_mathjax:\n
#+html_equation_reference_format: \eqref{%s}\n
#+html_head:\n
#+html_head_extra:\n
#+subtitle:\n
#+infojs_opt:\n
#+latex_header: \n\n")
    (insert "#+BEGIN_EXPORT html \n <nav class=\"sidebar\"> \n#+END_EXPORT\n")
    (insert (create-global-toc-file-content plist (append excludelist '("./global-toc.org"))))
    (insert "#+BEGIN_EXPORT html \n </nav> \n#+END_EXPORT\n")))


(defun get-navbar-as-postamble (outdir)
  (with-temp-buffer
    (insert-file-contents (concat outdir "/global-toc.html"))
    (let ((body-start (search-forward "<nav class"))
          (body-end (search-forward "</nav>")))
      (buffer-substring (- body-start 10) body-end))))


(defun write-navbar-to-file-postamble (outdir file)
  "Write the navbar to the file postamble tag. If the navbar exists, do nothing.

TODO: maybe delete it?"
    (with-temp-file file
      (insert-file-contents file)
      (let ((has-sidebar (search-forward "<nav class=\"sidebar\">" nil t)))
        (unless has-sidebar
          (let ((postamble-start (re-search-forward "<div id=\"postamble\".*>")))
            (insert (get-navbar-as-postamble outdir))
            )))))


(defun set-postamble-function (plist)
  (let ((pubdir (plist-get plist :publishing-directory)))
    (mapcar
     (lambda (file) (write-navbar-to-file-postamble pubdir file))
     (seq-filter
      (lambda (f) (not (-contains? '("./global-toc.html" "index.html" "sitemap.html") f) ))
      (directory-files-recursively pubdir ".*html$")))))
