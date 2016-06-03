# distributed_quicksort

master loops through list of slaves and sends messages to idle slaves
slaves listen using tags to enforce ordering. first the param tag, then the data tag
work struct uses special values for start and end to signify wait/kill

start	end	action
-1		-1	wait
-1		0		kill
