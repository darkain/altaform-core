<?php


//FALL BACK IF random_int() DOESN'T EXIST - NEW IN PHP 7.0
//THIS IS USED FOR PASSWORD GENERATOR
if (!function_exists('random_int')) {
	function random_int($min, $max) { return mt_rand($min, $max); }
}
