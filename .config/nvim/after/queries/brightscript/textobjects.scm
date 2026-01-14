; extends

(sub_statement
	body: (block) @function.inner) @function.outer

(function_statement
	body: (block) @function.inner) @function.outer

(if_statement) @if.outer
(conditional_compl) @if.outer
(else_if_clause) @elseif.outer
(conditional_compl_else_if_clause) @elseif.outer
(else_clause) @else.outer
(conditional_compl_else_clause) @elseif.outer

(for_statement) @for.outer
(while_statement) @while.outer

(comment) @comment.outer

(string) @attribute.outer

(parameter) @parameter.outer
