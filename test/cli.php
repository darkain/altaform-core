<?php

//ALL WARNINGS AS EXCEPTIONS
error_reporting(E_ALL);
set_error_handler(function ($errno, $errstr, $errfile, $errline ) {
	throw new ErrorException($errstr, $errno, 0, $errfile, $errline);
});


//GENERIC FILLERS FOR UNIT TESTING
function is_owner($path) { return $path; }
class pudlObject {}
class pudlOrm {}
interface tbx_plugin {}


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

require_once(__DIR__ . '/../core/afVoid.inc.php');
if (empty($get))		$get		= new afVoid;

// AF 3.0 CODE
require_once(__DIR__ . '/../router/router.php');

// AF 2.0 CODE
require_once(__DIR__ . '/../traits/afCallable.inc.php');
require_once(__DIR__ . '/../traits/afNode.inc.php');

require_once(__DIR__ . '/../core/afDebug.inc.php');
require_once(__DIR__ . '/../core/afString.inc.php');
require_once(__DIR__ . '/../core/afCli.inc.php');
require_once(__DIR__ . '/../core/afStatus.inc.php');

//THESE MUST COME AFTER OTHERS AND BE INCLUDED IN THIS ORDER
require_once(__DIR__ . '/../core/afUrl.inc.php');
require_once(__DIR__ . '/../core/afUser.inc.php');

//AFCONFIG IS STAND ALONE
require_once(__DIR__ . '/../core/afConfig.inc.php');
if (empty($afconfig))	$afconfig	= new afConfig;



echo afCli::fgWhite("PHP Version:\t") . afCli::fgCyan(PHP_VERSION) . "\n\n";


require(__DIR__.'/all.php');


echo afCli::fgGreen(1,"\nSuccess:\t".$__af_test_total__);
echo afCli::fgGreen(' Altaform unit tests completed') . "\n\n";
