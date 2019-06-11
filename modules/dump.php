<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// OUR OWN CUSTOM DATA DUMP FUNCTION
////////////////////////////////////////////////////////////////////////////////
function dump($var, $end=true) {
	global $af;

	if ($var instanceof \pudlObject) $var = $var->raw();

	if (function_exists('\af\cli')  &&  cli()) {
		if (!empty($af)  &&  ($af instanceof altaform)) {
			$af->contentType('txt');
		}
		var_export($var);
		echo "\n";

	} else {
		echo '<pre>';
		var_export($var);
		echo '</pre>';
	}

	if ($end) exit(1);
}
