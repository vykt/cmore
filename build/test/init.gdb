set environment CK_FORK=no

define pnode
	if $argc != 1
		printf "Use: pnode <*node>\n"
	else
		printf "Node data:   "
		p *((int *) $arg0->data)
	end
end

define pcolour
	if $argc != 1
		printf "Use: pcolour <*node>\n"
	else
		printf "Node colour: "
		p $arg0->colour
	end
end

define pall
	if $argc != 1
		printf "Use: pall <*node>\n"
	else
		pnode $arg0
		pcolour $arg0
	end
end

tb main
run
