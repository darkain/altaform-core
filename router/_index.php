<?php



////////////////////////////////////////////////////////////////////////////////
// PARSE THE URL AND LOAD THE PAGE!
// THIS IS THE MAIN PART OF THE INIT SCRIPT THAT RUNS THE APPLICATION CODE
////////////////////////////////////////////////////////////////////////////////

while ($router->reparse) {
	$router->reparse	= false;
	$router->path		= $router->route($af);

	if (is_string($router->path)  &&  $router->path !== '') {
		if ($af->debug()) {
			echo '<!-- BEGIN: ' . getcwd() . $router->path . " -->\n";
			require(is_owner($router->path));
			echo '<!-- END: ' . getcwd() . $router->path . " -->\n";
		} else {
			require(is_owner($router->path));
		}
	}

	chdir($router->directory);
}
