<?php

//Enable PHP Output Buffering
ob_start();



//PHP Error handling functions
require_once('error.php.inc');



//Set the internal character encoding to UTF-8
mb_http_output('UTF-8');
mb_regex_encoding('UTF-8');
mb_internal_encoding('UTF-8');



//Set the include path to make it easier to use include() and require()
set_include_path(get_include_path() . PATH_SEPARATOR . getcwd());



//we DONT need compression from PHP itself
//web server / reverse proxy handles this task for us
ini_set('zlib.output_compression', 'Off');
ini_set('zlib.output_compression_level', '0');



//DEFINE PHP_VERSION_ID IF NOT ALREADY DEFINED
if (!defined('PHP_VERSION_ID')) {
	$version = explode('.', PHP_VERSION);
	define('PHP_VERSION_ID', ($version[0] * 10000 + $version[1] * 100 + $version[2]));
}



//Getvar Library (GET/POST variables)
require_once('_getvar/getvar.php.inc');
$get = new getvar;



//Main Library
require_once('common.php.inc');



//Altaform Base Code
require_once('altaform.php.inc');



//Altaform Object Types
require_once('types.php.inc');



//URL Parser
require_once('url.php.inc');



//Set the content type for this document to HTML with UTF-8 encoding
if (!headers_sent()) {
	if (strpos($get->server('HTTP_USER_AGENT'), ' Trident/')) {
		header('X-UA-Compatible: IE=edge,chrome=1');
	}
	header('Content-Language: en_US');
	header('Content-Type: text/html; charset=utf-8');
	header('X-Content-Type-Options: nosniff');
}



//Host Information for Config
assert500(afurl::validateDomain($afurl->domain), 'Invalid Domain: ' . $afurl->domain);



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



//Initialize afurl
$afurl->_all();



//Upgrade to HTTPS connection (modern)
if ($get->server('HTTP_UPGRADE_INSECURE_REQUESTS')) {
	if ($afurl->https) {
		$afurl->secure(isset($site['secure']) ? $site['secure'] : false);
	} else if (!empty($site['secure'])) {
		$afurl->redirectSecure();
	}

//Upgrade to HTTPS connection (legacy)
} else if (!$afurl->https  &&  !empty($site['secure'])) {
	$agent		= strtolower($get->server('HTTP_USER_AGENT'));

	$secure_ua	= [	'trident/',		'firefox/',		'applewebkit/',
					'konqueror/',	'iceweasel/',
					'bot/',			'spider/'];

	foreach ($secure_ua as $ua) {
		if (strpos($agent, $ua)  &&  !strpos($agent, 'windows nt 5.')) {
			$afurl->redirectSecure();
		}
	}

//HSTS - HTTP Strict Transport Security
} else if ($afurl->https) {
	$afurl->secure(isset($site['secure']) ? $site['secure'] : false);
}



//Disable Frames
if (!headers_sent()) {
	header(
		'X-Frame-Options: ' .
		(empty($site['frames']) ? 'sameorigin' : $site['frames'])
	);
}



//Set the time zone and locale
date_default_timezone_set(
	empty($site['timezone']) ? 'UTC' : $site['timezone']
);

setlocale(LC_CTYPE,
	empty($site['ctype']) ? 'en_US.UTF-8' : $site['ctype']
);



//HTTP Options / origins
if (!empty($site['origins'])) {
	if (in_array($get->server('HTTP_ORIGIN'), $site['origins'])) {
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
if (!empty($site['pudl'])  &&  tbx_array($site['pudl'])) {
	require_once('_pudl/pudl.php');
	require_once('_pudl/pudlSession.php');
	require_once('user.php.inc');
	$db = pudl::instance($site['pudl']);

	$db->on('log',		'_pudl_log');
	$db->on('debug',	'_pudl_debug');
	$get->pudl = $db;

	if (!empty($site['pudl']['connected'])) {
		call_user_func($site['pudl']['connected']);
	}

	unset($site['pudl']);

	$db->time(
		$af = altaform::create(
			new pudlSession($db, 'pudl_session',
				isset($site['session_name'])	? $site['session_name']		: false,
				isset($site['session_domain'])	? $site['session_domain']	: false,
				$afurl->https
			)
		)
	);

} else {
	require_once('user.php.inc');
	$af = altaform::create();
}




//Process current user session
$af->login();



//Parse the URL and load the page!
while ($afurl->reparse) {
	$afurl->reparse = false;
	$afurl->path = $afurl->process();
	if (is_string($afurl->path)) require($afurl->path);
	chdir($afurl->directory);
}



//Flush PHP Output Buffer
$i=20;
while ($i--  &&  ob_get_level()){
	ob_end_flush();
}
