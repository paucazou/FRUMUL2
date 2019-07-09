" Thx: https://code.i-harness.com/en/q/4bf088

let s:path = expand('<sfile>:p:h')

function! HiTags()
	pyx << EOF
import sys
import vim
path = vim.eval('s:path')
sys.path.append(path)
from get_tags import hi_tags
hi_tags()
EOF
endfunction

command! HiTags call HiTags()
