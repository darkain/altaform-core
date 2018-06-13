<?php

//ALL WARNINGS AS EXCEPTIONS
error_reporting(E_ALL);
function exception_error_handler($errno, $errstr, $errfile, $errline ) {
	throw new ErrorException($errstr, $errno, 0, $errfile, $errline);
}
set_error_handler("exception_error_handler");



//PHP REQUIRES DEFAULT TIMEZONE TO BE SET NOW
date_default_timezone_set('UTC');



//SET FLOATING POINT SERIALIZATION PRECISION TO A KNOWN VALUE
ini_set('precision', 14);
ini_set('serialize_precision', 14);



//CHANGE TO THIS DIRECTORY FOR CONSISTENCY
chdir(__DIR__);



require_once(__DIR__ . '/../core/afString.inc.php');


echo "PHP:\t" . PHP_VERSION . "\n";


require(__DIR__.'/all.php');


echo "ALL GOOD!!\n";
