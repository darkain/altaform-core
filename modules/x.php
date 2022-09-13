<?php


////////////////////////////////////////////////////////////////////////////////
// FALL BACK IF random_int() DOESN'T EXIST - NEW IN PHP 7.0
// THIS IS A POLYFILL FOR PHP 5.X - REALLY THOUGH, PLEASE UPGRADE TO 8.X
/** @suppress PhanRedefineFunctionInternal */
////////////////////////////////////////////////////////////////////////////////
if (!function_exists('random_int')) {
	function random_int($min, $max) {
		return mt_rand($min, $max);
	}
}
