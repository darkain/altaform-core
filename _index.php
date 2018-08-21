<?php


// DISABLE DOUBLE-LOADING
if (class_exists('altaform', false)) return;


//Enable automatic error display
ini_set('display_errors', 'on');




//Set the include path to make it easier to use include() and require()
chdir(__DIR__.'/..');
set_include_path(get_include_path() . PATH_SEPARATOR . getcwd());




//PHP Error handling functions
require_once(__DIR__.'/core/afError.inc.php');




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




//Getvar Library (GET/POST variables)
require_once('_getvar/getvar.inc.php');
$get = new getvar;



//Altaform Base Code
require_once(__DIR__.'/includes.inc.php');



//URL Parser
require_once(__DIR__.'/core/afUrl.inc.php');



//If we're on the CLI, disable output buffering
if (afCli()  &&  ob_get_level()) ob_end_flush();



//Set the content type for this document to HTML or TXT with UTF-8 encoding
if (!headers_sent()) {
	if (afDevice::trident()) {
		header('X-UA-Compatible: IE=edge,chrome=1');
	}

	header('Feature-Policy: lazyload *');
	header('X-Content-Type-Options: nosniff');
	header('Content-Language: en_US');

	if (afCli()) {
		header('Content-Type: text/plain; charset=utf-8');
	} else {
		header('Content-Type: text/html; charset=utf-8');
	}
}




//Host Information for Config
if (!afCli()) {
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
if (is_file($afconfig->root . '/_altaform.inc.php')) {
	$__cwd = getcwd();
	chdir($afconfig->root);
	require('_altaform.inc.php');
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
	if (afDevice::secure()) {
		$afurl->redirectSecure();
	}

//HSTS - HTTP Strict Transport Security
} else if ($afurl->https) {
	$afurl->secure($afconfig->secure);
}




////////////////////////////////////////////////////////////////////////////////
// DISABLE FRAMES
////////////////////////////////////////////////////////////////////////////////
if (!headers_sent()) {
	header('X-Frame-Options: ' . $afconfig->frames);
}




////////////////////////////////////////////////////////////////////////////////
// SET THE TIME ZONE AND LOCALE (LOCALIZATION)
////////////////////////////////////////////////////////////////////////////////
date_default_timezone_set(	$afconfig->timezone);
setlocale(LC_CTYPE,			$afconfig->locale);




////////////////////////////////////////////////////////////////////////////////
// HTTP OPTIONS / ORIGINS
////////////////////////////////////////////////////////////////////////////////
if (!empty($afurl->origin)) {
	if (in_array($afurl->origin, $afconfig->origins)) {
		header('Access-Control-Allow-Origin: ' . $afurl->origin);
		header('Access-Control-Allow-Methods: POST, GET, OPTIONS');
		header('Access-Control-Allow-Headers: Content-Type, Cache-Control, X-Requested-With');
		header('Access-Control-Allow-Credentials: true');
	}
	if (strtolower($get->server('REQUEST_METHOD')) === 'options') {
		return;
	}
}




////////////////////////////////////////////////////////////////////////////////
// INITIALIZE THE DATABASE CONNECTION
////////////////////////////////////////////////////////////////////////////////
if (!empty($afconfig->pudl)  &&  tbx_array($afconfig->pudl)) {
	require_once('_pudl/pudl.php');
	require_once('_pudl/pudlSession.php');
	require_once(__DIR__.'/core/afUser.inc.php');

	if (afCli()) $afconfig->pudl['timeout'] = AF_DAY;

	$db = pudl::instance($afconfig->pudl);

	$db->on('log',		'afPudlLog');

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

	$af->settings = $db	->cache(AF_MINUTE*5, 'altaform_settings')
						->collection('pudl_altaform');

} else {
	require_once(__DIR__.'/core/afUser.inc.php');
	$af = altaform::create();
}




////////////////////////////////////////////////////////////////////////////////
// EXTENDED TIMEOUT VALUE
////////////////////////////////////////////////////////////////////////////////
if (afCli()) $af->timeout(AF_DAY);




////////////////////////////////////////////////////////////////////////////////
// PROCESS CURRENT USER SESSION
////////////////////////////////////////////////////////////////////////////////
$af->login();




////////////////////////////////////////////////////////////////////////////////
// PARSE THE URL AND LOAD THE PAGE!
// THIS IS THE MAIN PART OF THE INIT SCRIPT THAT RUNS THE APPLICATION CODE
////////////////////////////////////////////////////////////////////////////////
while ($afrouter->reparse) {
	$afrouter->reparse = false;
	$afrouter->path = $afrouter->route($af);
	if (is_string($afrouter->path)) require($afrouter->path);
	chdir($afrouter->directory);
}




////////////////////////////////////////////////////////////////////////////////
// FLUSH PHP OUTPUT BUFFER
////////////////////////////////////////////////////////////////////////////////
$i = 20;
while ($i--  &&  ob_get_level()) {
	ob_end_flush();
}




////////////////////////////////////////////////////////////////////////////////
// NEW LINE FOR CLI MODE
////////////////////////////////////////////////////////////////////////////////
if (afCli()) echo "\n";
