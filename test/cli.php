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


//HANDLE CHECKING IF ITEM IS REALLY AN ARRAY OR NOT
function tbx_array($item) {
	if (is_array($item)) return true;
	return ($item instanceof ArrayAccess);
}


//CHANGE TO THIS DIRECTORY FOR CONSISTENCY
chdir(__DIR__);


require_once(__DIR__ . '/../core/afError.inc.php');
require_once(__DIR__ . '/../core/afCli.inc.php');

require_once(__DIR__ . '/../traits/afCallable.inc.php');
require_once(__DIR__ . '/../traits/afRouter.inc.php');

require_once(__DIR__ . '/../core/afVoid.inc.php');
if (empty($get))	$get	= new afVoid;

require_once(__DIR__ . '/../core/afString.inc.php');
require_once(__DIR__ . '/../core/afUrl.inc.php');




echo "PHP:\t" . PHP_VERSION . "\n";


require(__DIR__.'/all.php');


echo $afUnit . ' Altaform unit tests completed successfully' . "\n";
