<?php



//Enable automatic error display
ini_set('display_errors', 'on');




//Set the include path to make it easier to use include() and require()
chdir(__DIR__.'/..');
set_include_path(get_include_path() . PATH_SEPARATOR . getcwd());




//PHP Error handling functions
require_once('error.php.inc');




//Enable PHP Output Buffering
ob_start();




//Set the internal character encoding to UTF-8
if (extension_loaded('mbstring')) {
	mb_http_output('UTF-8');
	mb_regex_encoding('UTF-8');
	mb_internal_encoding('UTF-8');
}




//we DONT need compression from PHP itself
//web server / reverse proxy handles this task for us
ini_set('zlib.output_compression', 'Off');
ini_set('zlib.output_compression_level', '0');




//DEFINE PHP_VERSION_ID IF NOT ALREADY DEFINED
if (!defined('PHP_VERSION_ID')) {
	$version = explode('.', PHP_VERSION);
	define('PHP_VERSION_ID', ($version[0] * 10000 + $version[1] * 100 + $version[2]));
}




//FIX FOR PHP 7.1 FLOATING POINT PRECISION DURING SERIALIZATION
//https://stackoverflow.com/questions/42981409/php7-1-json-encode-float-issue
if (version_compare(phpversion(), '7.1', '>=')) {
	ini_set( 'serialize_precision', -1 );
}




//PUDL Interfaces
require_once('_pudl/pudlInterfaces.php');



//Getvar Library (GET/POST variables)
require_once('_getvar/getvar.php.inc');
$get = new getvar;



//Main Library
require_once('common.php.inc');



//Altaform Base Code
require_once('_altaform.php.inc');



//URL Parser
require_once('af_url.php.inc');



//If we're on the CLI, disable output buffering
if (altaform::cli()) ob_end_flush();



//Set the content type for this document to HTML or TXT with UTF-8 encoding
if (!headers_sent()) {
	if (afdevice::trident()) {
		header('X-UA-Compatible: IE=edge,chrome=1');
	}

	header('X-Content-Type-Options: nosniff');
	header('Content-Language: en_US');

	if (altaform::cli()) {
		header('Content-Type: text/plain; charset=utf-8');
	} else {
		header('Content-Type: text/html; charset=utf-8');
	}
}




//Host Information for Config
if (!altaform::cli()) {
	assert500(
		$afurl->validateDomain($afurl->domain),
		'Invalid Domain: ' . $afurl->domain
	);
}




//Main Configuration File
if (is_file('_config/'.$afurl->domain.'/config.php.inc')) {
	require_once('_config/'.$afurl->domain.'/config.php.inc');
} else if (is_file('_config/'.$afurl->domain)) {
	require_once('_config/'.$afurl->domain);
} else if (is_file('_config/_virtual/config.php.inc')) {
	require_once('_config/_virtual/config.php.inc');
} else if (is_file('_config/_virtual.php.inc')) {
	require_once('_config/_virtual.php.inc');
} else {
	error500('Unknown Domain: ' . $afurl->domain);
}




//Load additional configuation file
if (is_file($afconfig->root . '/_altaform.php.inc')) {
	$__cwd = getcwd();
	chdir($afconfig->root);
	require('_altaform.php.inc');
	chdir($__cwd);
	unset($__cwd);
}




//Do post-load configuration updates
$afconfig->_onload();




//Set default flags for Getvar
if (isset($afconfig->getvar)) {
	$get->flags($afconfig->getvar);
}




//Initialize afurl
$afurl->_all();




//Disable implicit debugging
if (is_object($afconfig->debug)) {
	$afconfig->debug = false;
}




//Disable automatic error display
ini_set('display_errors', 'off');




//Upgrade to HTTPS connection (modern)
if ($get->server('HTTP_UPGRADE_INSECURE_REQUESTS')) {
	if ($afurl->https) {
		$afurl->secure($afconfig->secure);
	} else if (!empty($afconfig->secure)) {
		$afurl->redirectSecure();
	}

//Upgrade to HTTPS connection (legacy)
} else if (!$afurl->https  &&  !empty($afconfig->secure)) {
	if (afdevice::is_secure()) {
		$afurl->redirectSecure();
	}

//HSTS - HTTP Strict Transport Security
} else if ($afurl->https) {
	$afurl->secure($afconfig->secure);
}




//Disable Frames
if (!headers_sent()) {
	header('X-Frame-Options: ' . $afconfig->frames);
}




//Set the time zone and locale (localization)
date_default_timezone_set(	$afconfig->timezone);
setlocale(LC_CTYPE,			$afconfig->locale);




//HTTP Options / origins
if (!empty( $get->server('HTTP_ORIGIN') )) {
	if (in_array($get->server('HTTP_ORIGIN'), $afconfig->origins)) {
		header('Access-Control-Allow-Origin: '.$get->server('HTTP_ORIGIN'));
		header('Access-Control-Allow-Methods: POST, GET, OPTIONS');
		header('Access-Control-Allow-Headers: Content-Type, Cache-Control, X-Requested-With');
		header('Access-Control-Allow-Credentials: true');
	}
	if (strtolower($get->server('REQUEST_METHOD')) === 'options') {
		return;
	}
}



//Initialize all the things!
if (!empty($afconfig->pudl)  &&  tbx_array($afconfig->pudl)) {
	require_once('_pudl/pudl.php');
	require_once('_pudl/pudlSession.php');
	require_once('af_user.php.inc');
	$db = pudl::instance($afconfig->pudl);

	$db->on('log',		'_pudl_log');

	if (!empty($afconfig->pudl['connected'])) {
		call_user_func($afconfig->pudl['connected']);
	}

	//Hide PUDL config from $afconfig
	$afconfig->pudl = [];

	$db->time(
		$af = altaform::create(
			new pudlSession($db, 'pudl_session',
				$afconfig->session['name'],
				$afconfig->session['domain'],
				$afurl->https
			)
		)
	);

	if (altaform::cli()) $db->timeout(AF_DAY);

	$af->settings = $db	->cache(AF_MINUTE*5, 'altaform_settings')
						->collection('pudl_altaform');

} else {
	require_once('af_user.php.inc');
	$af = altaform::create();
}




//Process current user session
$af->login();




//Parse the URL and load the page!
while ($afurl->reparse) {
	$afurl->reparse = false;
	$afurl->path = $afurl->route();
	if (is_string($afurl->path)) require($afurl->path);
	chdir($afurl->directory);
}




//Flush PHP Output Buffer
$i = 20;
while ($i--  &&  ob_get_level()) {
	ob_end_flush();
}



//New line for CLI mode
if (altaform::cli()) echo "\n";
