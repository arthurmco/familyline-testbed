(load-file "./docgen.el")

(defun prepare-doc-information (plist)
  (create-global-toc-file plist '("./sitemap.org" "./theindex.org"))
  plist)

(setq org-publish-project-alist
      '(("docpages"
         :base-directory "."
         :base-extension "org"
         :publishing-directory "./out/"
         :publishing-function org-html-publish-to-html
         :preparation-function (prepare-doc-information)
         :completion-function (set-postamble-function)
         :html-preamble t
         :section-numbers nil
         :headline-levels 3
         :html-head-extra "<link rel=\"stylesheet\" href=\"styles/menu.css\" />"
         :html-html5-fancy t
         :html-postamble t
         :html-postamble-format
         (("en" "<hr/><p class=\"date\">Last updated at %C.</p>"))
         :with-toc nil)
        
        ("images"
         :base-directory "."
         :base-extension "jpg\\|gif\\|png"
         :publishing-directory "./out/"
         :publishing-function org-publish-attachment)

        ("styles"
         :base-directory "."
         :base-extension "css"
         :publishing-directory "./out/styles"
         :publishing-function org-publish-attachment)

        ("familyline-docs" :components ("docpages" "images" "styles"))))
