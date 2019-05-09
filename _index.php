<?php


////////////////////////////////////////////////////////////////////////////////
// DISABLE DOUBLE-LOADING
////////////////////////////////////////////////////////////////////////////////
if (class_exists('altaform', false)) return;




////////////////////////////////////////////////////////////////////////////////
// ENABLE AUTOMATIC ERROR DISPLAY
////////////////////////////////////////////////////////////////////////////////
ini_set('display_errors', 'on');




////////////////////////////////////////////////////////////////////////////////
// SET THE INCLUDE PATH TO MAKE IT EASIER TO USE INCLUDE() AND REQUIRE()
////////////////////////////////////////////////////////////////////////////////
chdir(__DIR__.'/..');
set_include_path(get_include_path() . PATH_SEPARATOR . getcwd());




////////////////////////////////////////////////////////////////////////////////
// CREATE DUMMY IS_OWNER() FUNCTION IF IT DOESNT ALREADY EXIST
////////////////////////////////////////////////////////////////////////////////
if (!function_exists('is_owner')) {
	/** @suppress PhanRedefineFunction */
	function is_owner($path) { return $path; }
}




////////////////////////////////////////////////////////////////////////////////
// PHP ERROR HANDLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
require_once(is_owner(__DIR__.'/core/afDebug.inc.php'));




////////////////////////////////////////////////////////////////////////////////
// ENABLE PHP OUTPUT BUFFERING
////////////////////////////////////////////////////////////////////////////////
ob_start();




////////////////////////////////////////////////////////////////////////////////
// SET THE INTERNAL CHARACTER ENCODING TO UTF-8
////////////////////////////////////////////////////////////////////////////////
if (extension_loaded('mbstring')) {
	mb_http_output('UTF-8');
	mb_regex_encoding('UTF-8');
	mb_internal_encoding('UTF-8');
}




////////////////////////////////////////////////////////////////////////////////
// WE DONT NEED COMPRESSION FROM PHP ITSELF
// WEB SERVER / REVERSE PROXY HANDLES THIS TASK FOR US
////////////////////////////////////////////////////////////////////////////////
if (!headers_sent()) {
	@ini_set('zlib.output_compression', 'Off');
	@ini_set('zlib.output_compression_level', '0');
}




////////////////////////////////////////////////////////////////////////////////
// FIX FOR PHP 7.1 FLOATING POINT PRECISION DURING SERIALIZATION
// https://stackoverflow.com/questions/42981409/php7-1-json-encode-float-issue
////////////////////////////////////////////////////////////////////////////////
if (version_compare(phpversion(), '7.1', '>=')) {
	ini_set('serialize_precision', -1);
}




////////////////////////////////////////////////////////////////////////////////
// GETVAR LIBRARY (GET/POST VARIABLES)
////////////////////////////////////////////////////////////////////////////////
require_once(is_owner('_getvar/getvar.inc.php'));
$get = new getvar;




////////////////////////////////////////////////////////////////////////////////
// ALTAFORM BASE CODE
////////////////////////////////////////////////////////////////////////////////
require_once(is_owner(__DIR__.'/includes.inc.php'));




////////////////////////////////////////////////////////////////////////////////
// URL PARSER
////////////////////////////////////////////////////////////////////////////////
require_once(is_owner(__DIR__.'/core/afUrl.inc.php'));




////////////////////////////////////////////////////////////////////////////////
// IF WE'RE ON THE CLI, DISABLE OUTPUT BUFFERING
////////////////////////////////////////////////////////////////////////////////
if (\af\cli()  &&  ob_get_level()) ob_end_flush();




////////////////////////////////////////////////////////////////////////////////
// SET THE CONTENT TYPE FOR THIS DOCUMENT TO HTML OR TXT WITH UTF-8 ENCODING
////////////////////////////////////////////////////////////////////////////////
if (!headers_sent()) {
	if (afDevice::trident()) {
		header('X-UA-Compatible: IE=edge,chrome=1');
	}

	header('X-Content-Type-Options: nosniff');
	header('Content-Language: en_US');

	if (\af\cli()) {
		header('Content-Type: text/plain; charset=utf-8');
	} else {
		header('Content-Type: text/html; charset=utf-8');
	}
}




////////////////////////////////////////////////////////////////////////////////
// HOST INFORMATION FOR CONFIG
////////////////////////////////////////////////////////////////////////////////
if (!\af\cli()) {
	assertStatus(500,
		$afurl->validateDomain($afurl->domain),
		'Invalid Domain: ' . $afurl->domain
	);
}




////////////////////////////////////////////////////////////////////////////////
// MAIN CONFIGURATION FILE
////////////////////////////////////////////////////////////////////////////////
if (is_file('_config/'.$afurl->domain.'/config.inc.php')) {
	require_once(is_owner('_config/'.$afurl->domain.'/config.inc.php'));
} else if (is_file('_config/'.$afurl->domain)) {
	require_once(is_owner('_config/'.$afurl->domain));
} else if (is_file('_config/_virtual/config.inc.php')) {
	require_once(is_owner('_config/_virtual/config.inc.php'));
} else if (is_file('_config/_virtual.inc.php')) {
	require_once(is_owner('_config/_virtual.inc.php'));
} else {
	httpError(500, 'Unknown Domain: ' . $afurl->domain);
}




////////////////////////////////////////////////////////////////////////////////
// LOAD ADDITIONAL CONFIGUATION FILE
////////////////////////////////////////////////////////////////////////////////
if (is_file($afconfig->root . '/_altaform.php')) {
	$__af_cwd__ = getcwd();
	chdir($afconfig->root);
	require(is_owner('_altaform.php'));
	chdir($__af_cwd__);
	unset($__af_cwd__);
}




////////////////////////////////////////////////////////////////////////////////
// SET THE TIME ZONE AND LOCALE (LOCALIZATION)
////////////////////////////////////////////////////////////////////////////////
date_default_timezone_set(	$afconfig->timezone);
setlocale(LC_CTYPE,			$afconfig->locale);




////////////////////////////////////////////////////////////////////////////////
// DO POST-LOAD CONFIGURATION UPDATES
////////////////////////////////////////////////////////////////////////////////
$afconfig->_onload();




////////////////////////////////////////////////////////////////////////////////
// CREATE GEOLOCATION INSTANCE
////////////////////////////////////////////////////////////////////////////////
$geo = new \af\geo($afconfig);




////////////////////////////////////////////////////////////////////////////////
// SET DEFAULT FLAGS FOR GETVAR
////////////////////////////////////////////////////////////////////////////////
if (isset($afconfig->getvar)) {
	$get->flags($afconfig->getvar);
}




////////////////////////////////////////////////////////////////////////////////
// INITIALIZE AFURL
////////////////////////////////////////////////////////////////////////////////
$afurl->_all($router);




////////////////////////////////////////////////////////////////////////////////
// DISABLE IMPLICIT DEBUGGING
////////////////////////////////////////////////////////////////////////////////
if (is_object($afconfig->debug)) {
	$afconfig->debug = false;
}




////////////////////////////////////////////////////////////////////////////////
// DISABLE AUTOMATIC ERROR DISPLAY
////////////////////////////////////////////////////////////////////////////////
ini_set('display_errors', 'off');




////////////////////////////////////////////////////////////////////////////////
// UPGRADE TO HTTPS CONNECTION
////////////////////////////////////////////////////////////////////////////////

// MODERN BROWSERS
if ($get->server('HTTP_UPGRADE_INSECURE_REQUESTS')) {
	if ($afurl->https) {
		$afurl->secure($afconfig->secure);
	} else if (!empty($afconfig->secure)) {
		$afurl->redirectSecure();
	}

// LEGACY BROWSERS
} else if (!$afurl->https  &&  !empty($afconfig->secure)) {
	if (afDevice::secure()) {
		$afurl->redirectSecure();
	}

// HSTS - HTTP STRICT TRANSPORT SECURITY
} else if ($afurl->https) {
	$afurl->secure($afconfig->secure);
}




////////////////////////////////////////////////////////////////////////////////
// DISABLE FRAMES, XSS, MODIFY REFERRER, AND USE A CUSTOM FEATURE POLICY
////////////////////////////////////////////////////////////////////////////////
if (!headers_sent()) {
	header('X-Frame-Options: '	. $afconfig->frames);
	header('X-XSS-Protection: '	. $afconfig->xss);
	header('Referrer-Policy: '	. $afconfig->referrer);

	$policy = '';
	foreach ($afconfig->features as $key => $value) {
		if (!empty($policy)) $policy .= '; ';
		$policy .= $key . ' ' . $value;
	}
	header('Feature-Policy: '	. $policy);
	unset($policy, $key, $value);
}




////////////////////////////////////////////////////////////////////////////////
// HTTP OPTIONS / ORIGINS
////////////////////////////////////////////////////////////////////////////////
if (!empty($afurl->origin)  &&  !headers_sent()) {
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
	require_once(is_owner('_pudl/pudl.php'));
	require_once(is_owner('_pudl/pudlSession.php'));
	require_once(is_owner(__DIR__.'/core/afUser.inc.php'));

	if (\af\cli()) $afconfig->pudl['timeout'] = AF_DAY;

	$db = pudl::instance($afconfig->pudl);

	$db->on('log', 'afPudlLog');

	if (!empty($afconfig->pudl['connected'])) {
		if (is_callable($afconfig->pudl['connected'])) {
			call_user_func($afconfig->pudl['connected']);
		}
	}

	// HIDE PUDL CONFIG FROM $afconfig
	$afconfig->pudl = [];

	$db->time(
		$af = altaform::create(
			new pudlSession($db, 'session',
				$afconfig->session + [
					'secure' => $afurl->https,
				]
			)
		)
	);

	// LOAD ALTAFORM SETTINGS
	try {
		$af->settings = $db	->cache(AF_MINUTE*5, 'altaform_settings')
							->collection('pudl_altaform');
	} catch (pudlException $e) {
		$af->settings = [];
	}

} else {
	require_once(is_owner('_pudl/pudl.php'));
	require_once(is_owner(__DIR__.'/core/afUser.inc.php'));
	$af = altaform::create();
}




////////////////////////////////////////////////////////////////////////////////
// EXTENDED TIMEOUT VALUE
////////////////////////////////////////////////////////////////////////////////
if (\af\cli()) $af->timeout(AF_DAY);




////////////////////////////////////////////////////////////////////////////////
// PROCESS CURRENT USER SESSION
////////////////////////////////////////////////////////////////////////////////
$af->login();




////////////////////////////////////////////////////////////////////////////////
// ROUTE THE REQUEST AND RUN THE APPLICATION
////////////////////////////////////////////////////////////////////////////////
require('router/_index.php');




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
if (\af\cli()) echo "\n";
