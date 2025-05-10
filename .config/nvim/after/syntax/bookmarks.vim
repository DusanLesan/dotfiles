syntax match BookmarkValidLine /^'[^']\+'\s\+\S\+/ contains=BookmarkPattern,BookmarkPath
syntax match BookmarkPattern /^'[^']\+'/ contained
syntax match BookmarkPath /\s\zs\S\+$/ contained
syntax match BookmarkComment /^[^'].*$/
highlight def link BookmarkValidLine NONE
highlight def link BookmarkPattern String
highlight def link BookmarkPath Directory
highlight def link BookmarkComment Comment
