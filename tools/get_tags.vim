function! HiTags()
	pyx << EOF
import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
from get_tags import hi_tags
hi_tags()
EOF
endfunction

command! HiTags call HiTags()
