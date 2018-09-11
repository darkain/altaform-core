<?php


////////////////////////////////////////////////////////////////////////////////
//FALL BACK IF random_int() DOESN'T EXIST - NEW IN PHP 7.0
//THIS IS USED FOR PASSWORD GENERATOR
/** @suppress PhanRedefineFunctionInternal */
////////////////////////////////////////////////////////////////////////////////
if (!function_exists('random_int')) {
	function random_int($min, $max) { return mt_rand($min, $max); }
}




////////////////////////////////////////////////////////////////////////////////
//CASE INSENSITIVE IN_ARRAY()
//SOURCE: http://us2.php.net/manual/en/function.in-array.php#89256
////////////////////////////////////////////////////////////////////////////////
if (!function_exists('in_arrayi')) {
	function in_arrayi($needle, $haystack) {
		return in_array(
			strtolower($needle),
			array_map('strtolower', $haystack)
		);
	}
}
