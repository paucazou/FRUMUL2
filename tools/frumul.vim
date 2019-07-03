" Vim syntax file
" Language: FRUMUL 2

if exists("b:current_syntax")
	finish
endif


syn keyword FRUMULheaderKWvalues_programmatic_types contained int text bool symbol list static const
syn keyword FRUMULheaderKWvalues_programmatic_bools contained true false
syn keyword FRUMULheaderKWvalues_programmatic_statements contained loop pool if fi
syn match FRUMULheaderKWvalues_programmatic_ints contained '\d\+'
syn match FRUMULheaderKWvalues_programmatic_ints contained '[-+]\d+' 
syn match FRUMULheaderKWvalues_ints contained '\d\+'
syn match FRUMULheaderKWvalues_ints contained '[-+]\d+' 
syn keyword FRUMULheaderKW contained namespace file alias bin 
syn keyword FRUMULheaderKW contained arg mark return lang
syn match FRUMULprogrammaticDelimiters '\:' contained
syn match FRUMULprogrammaticDelimiters '+' contained
syn match FRUMULprogrammaticDelimiters '-' contained
syn match FRUMULprogrammaticDelimiters '*' contained 
syn match FRUMULprogrammaticDelimiters '/' contained
syn match FRUMULprogrammaticDelimiters '%' contained
syn match FRUMULheaderDelimiters '\:' contained
"syn match FRUMULheaderDelimiters '¦' contained 
"syn match FRUMULheader_values_operators '+' contained
"syn match FRUMULheader_values_operators '-' contained
"syn match FRUMULheader_values_operators '/' contained
"syn match FRUMULheader_values_operators '\|' contained
"syn match FRUMULheader_values_operators '&' contained
"syn match FRUMULheader_values_operators '%' contained
"syn match FRUMULheader_values_operators '!' contained
"syn match FRUMULheader_values_operators '*' contained
": , ¦ = < > + - * / % & \| ! 
syn keyword FRUMULmain_tags ___HEADER___ ___TEXT___

syn region FRUMULheaderComment start='//*' end='*//' 
syn region string start='«' end='»' contained

syn region programmatic start="{" end="}" fold transparent contains=FRUMULheaderKWvalues_programmatic_types,string,FRUMULheaderKWvalues_programmatic_bools,FRUMULprogrammaticDelimiters,FRUMULheader_values_operators,FRUMULheaderKWvalues_programmatic_ints
syn region value start="«" end="»" fold transparent contains=programmatic,FRUMULheaderKWvalues_ints
syn region ___header___ matchgroup=FRUMULmain_tags start="___header___" end="___text___" fold transparent contains=FRUMULheaderKW,value,FRUMULheaderDelimiters,FRUMULheaderComment,FRUMULheaderKWvalues_programmatic_ints

hi def link FRUMULheaderKW 	Statement
hi def link FRUMULheaderComment	Comment
hi def link FRUMULmain_tags	PreProc
hi def link FRUMULheaderKWvalues_programmatic_types Type
hi def link string	Constant
hi def link FRUMULheaderKWvalues_programmatic_bools Constant
hi def link FRUMULheaderKWvalues_programmatic_ints Constant
hi def link FRUMULheaderKWvalues_ints Constant
hi def link FRUMULheaderDelimiters	Delimiter
hi def link FRUMULprogrammaticDelimiters Delimiter
