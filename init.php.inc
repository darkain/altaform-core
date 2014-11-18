<?php
//PHP Error handling functions
require_once('error.php.inc');


//Set the include path to make it easier to use include() and require()
set_include_path(get_include_path() . PATH_SEPARATOR . getcwd());

//we DONT need compression from the web server itself
//squid reverse proxy handles this task for us
ini_set('zlib.output_compression', 'Off');
ini_set('zlib.output_compression_level', '0');



//Set the content type for this document to HTML with UTF-8 encoding
if (!headers_sent()) {
	if (!empty($_SERVER['HTTP_USER_AGENT'])  &&  strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== false) {
		header('X-UA-Compatible: IE=edge,chrome=1');
	}
	header('Content-Type: text/html; charset=utf-8');
	header('Content-Language: en_US');
}


//DEFINE PHP_VERSION_ID IF NOT ALREADY DEFINED
if (!defined('PHP_VERSION_ID')) {
	$version = explode('.', PHP_VERSION);
	define('PHP_VERSION_ID', ($version[0] * 10000 + $version[1] * 100 + $version[2]));
}


//Getvar Library (GET/POST variables)
require_once('_getvar/getvar.php.inc');
$get = new getvar();


//Main Library
require_once('common.php.inc');


//Altaform Base Code
require_once('altaform.php.inc');


//Host Information for Config
$host = $get->server('HTTP_HOST');
$pos = strpos($host, ':');
if ($pos) $host = substr($host, 0, $pos);


//Main Configuration File
if (!is_file("_config/$host/config.php.inc")) $host = 'default';
if (!is_file("_config/$host/config.php.inc")) {
	error500('Unknown Domain: ' . $get->server('HTTP_HOST'));
}
require_once("_config/$host/config.php.inc");


//Set the time zone and locale
if (!empty($site['timezone'])) {
	date_default_timezone_set($site['timezone']);
} else {
	date_default_timezone_set('UTC');
}

if (!empty($site['ctype'])) {
	setlocale(LC_CTYPE, $site['ctype']);
} else {
	setlocale(LC_CTYPE, 'en_US.UTF-8');
}


//PHP Universal Database Library (PUDL)
if (isset($site['pudl_server'])) {
	if (!isset($site['pudl_type'])) $site['pudl_type'] = 'MySqli';
	$pudltype = 'pudl' . $site['pudl_type'];
	require_once("_pudl/$pudltype.php");
	$db = $pudltype::instance($site);
	if (!empty($site['pudl_connected'])) call_user_func($site['pudl_connected']);
}




//Initialize all the things!
if (isset($db)) {
	$db->debugger('_pudl_debug');
	$get->pudl = $db;
	$get->default = _GETVAR_SQLSAFE;

	//Redis Session Handler
	if (!empty($site['session_type'])  &&  $site['session_type'] === 'redis') {
		require_once('_redis/redisSession.php');
		$rses = new redisSession(
			isset($site['session_name'])	? $site['session_name']		: false,
			isset($site['session_domain'])	? $site['session_domain']	: false
		);

	//PUDL Session Handler
	} else {

		require_once('_pudl/pudlSession.php');
		new pudlSession(
			$db,
			'pudl_session',
			isset($site['session_name'])	? $site['session_name']		: false,
			isset($site['session_domain'])	? $site['session_domain']	: false
		);
	}
}


//Instantiate Altaform class
$af = new $altaform();
$af->login();
