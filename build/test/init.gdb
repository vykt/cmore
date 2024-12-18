set environment CK_FORK=no

define plst
	if $argc != 1
		printf "Use: pnode <*node>\n"
	else
		printf "Node data: "
		p *((int *) $arg0->data)
	end
end

define pvct
	if $argc != 2
		printf "Use: pvct <*vct> <index>\n"
	else
		printf "Index data: "
		p *(((int *) ($arg0->data)) + $arg1)
	end
end

define prbt
	if $argc != 1
		printf "Use: prbt <*node>\n"
	else
		printf "Node data:   "
		p *((int *) $arg0->data)
		printf "Node colour: "
		p $arg0->colour
	end
end

tb main
run
