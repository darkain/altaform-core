<?php



////////////////////////////////////////////////////////////////////////////////
// PARSE THE URL AND LOAD THE PAGE!
// THIS IS THE MAIN PART OF THE INIT SCRIPT THAT RUNS THE APPLICATION CODE
////////////////////////////////////////////////////////////////////////////////

while ($afrouter->reparse) {
	$afrouter->reparse	= false;
	$afrouter->path		= $afrouter->route($af);

	if (is_string($afrouter->path)  &&  $afrouter->path !== '') {
		require(is_owner($afrouter->path));
	}

	chdir($afrouter->directory);
}
