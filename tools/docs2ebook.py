#!/usr/bin/python

# Converts Familyline documentation into a ebook format
# You need to run it in the docs/ folder
#
# Copyright (C) 2018 Arthur M

# Really, just convert any .md file list into .ebook

import subprocess

lsout = subprocess.check_output('ls').decode()
filelist = lsout.strip().split('\n')

tmpdest = '/tmp/tempbase'
subprocess.call(['mkdir', '-p', '/tmp/tempbase/oebps/', '/tmp/tempbase/META-INF'])

# Write the mimetype
emime = open('%s/mimetype' % tmpdest, 'w')
emime.write('application/epub+zip')
emime.close()


# Convert markdown with html
for ffile in filelist:
    import os
    ccwd = os.getcwd()

    extension = ffile.strip().split('.')[-1]
    if extension == 'md':
        fileout = subprocess.check_output(['markdown', '-b', ccwd, '-f', 'fencedcode', ffile]).decode()
        filecontent = '<?xml version="1.0" encoding="UTF-8"?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"><html xmlns="http://www.w3.org/1999/xhtml"><head><title>.</title></head><body>%s</body></html>' % fileout

        htmlfile = open("%s/oebps/%s.xhtml" % (tmpdest, ffile), 'w');
        htmlfile.write(filecontent)
    else:
        subprocess.call(['cp', ffile, '%s/oebps/' % tmpdest])



# Create the container file
containerxml = open('%s/META-INF/container.xml' % tmpdest, 'w')
containerxml.write("""<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<container xmlns="urn:oasis:names:tc:opendocument:xmlns:container" version="1.0">
  <rootfiles>
    <rootfile full-path="oebps/content.opf" media-type="application/oebps-package+xml"/>
  </rootfiles>
</container>
""")
containerxml.close()

# Create the toc and the content.xml file
# The content lists all files, the ToC lists the titles
# Since they're basically the same thing, create both at one go

contentopf = open('%s/oebps/content.opf' % tmpdest, 'w')
tocncx = open('%s/oebps/toc.ncx' % tmpdest, 'w')

# No need to use a xml writer, for now
# Most things are hardcoded

contentopf.write("""<?xml version="1.0" encoding="utf-8" standalone="no"?>

<package xmlns="http://www.idpf.org/2007/opf" version="2.0" unique-identifier="bookid">
<metadata>
    <dc:identifier xmlns:dc="http://purl_org/dc/elements/1_1/" id="bookid">book.tribalia_repo_doc_code</dc:identifier>
    <dc:title xmlns:dc="http://purl.org/dc/elements/1.1/">Familyline Repo Documentation</dc:title>
    <dc:date xmlns:dc="http://purl.org/dc/elements/1.1/">2018-01-14</dc:date>
    <dc:creator xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:opf="http://www.idpf.org/2007/opf" opf:file-as="Mendes, Arthur">Arthur Mendes</dc:creator>
    <dc:rights xmlns:dc="http://purl.org/dc/elements/1.1/">Copyright © 2016-2018 </dc:rights>
    <dc:language xmlns:dc="http://purl.org/dc/elements/1.1/">en</dc:language>
</metadata>
<manifest>
    <item id="ncxtoc" media-type="application/x-dtbncx+xml" href="toc.ncx"/>
""")
tocncx.write("""<?xml version="1.0" encoding="utf-8" standalone="no"?>

<ncx xmlns="http://www.daisy.org/z3986/2005/ncx/" version="2005-1">
<head>
    <meta name="dtb:uid" content="_book.tribalia_repo_doc_code"/>
</head>
<docTitle>
    <text>Familyline Repo Documentation</text>
</docTitle>
<navMap>
""")

itemlist = []
itemreflist = []
tocnavmap = []

rorder = 1
for idx, ffile in enumerate(filelist):
    if not ffile.strip().endswith('md'):
        continue

    itemlist.append('<item id="iid%d" href="%s.xhtml" media-type="application/xhtml+xml"/>' % (idx, ffile) )
    itemreflist.append('<itemref idref="iid%d" />' % idx)

    # Get the title from the table of contents
    # The title is the first and topmost title
    title = ""
    with open("%s" % ffile, 'r') as offile:
        for fline in offile:
            if len(fline.strip()) < 4: # less than something meaningful
                continue

            title = fline.strip().replace('#', '', 1).strip()
            if len(title) > 1:
                break



    tocnavmap.append('<navPoint id="iid%d" playOrder="%d"><navLabel><text>%s</text></navLabel><content src="%s.xhtml"/></navPoint>' % (idx, rorder, title, ffile))
    rorder += 1
        

for it in itemlist:
    contentopf.write("\t%s\n" % it)

contentopf.write(
"""
</manifest>
<spine toc="ncxtoc">
""")

for it in itemreflist:
    contentopf.write("\t%s\n" % it)

contentopf.write(
"""
</spine>
</package>
""")

contentopf.close()

for it in tocnavmap:
    tocncx.write('\t%s\n' % it)

tocncx.write("""
    </navMap>
</ncx>""")
tocncx.close()

# Run the zip command
import os
olddir = os.getcwd()
os.chdir('%s' % tmpdest)
os.system('zip -X ebook.zip mimetype')
os.system('zip -X ebook.zip mimetype META-INF/* oebps/*')
os.chdir(olddir)

# Delete the temp dir
os.system('mv %s/ebook.zip ebook.epub' % tmpdest)


#subprocess.call(['rm', '-rf', '/tmp/tempbase'])
