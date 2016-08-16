<?php

ini_set('display_errors', 'off');

if (!empty($site['debug'])) error_reporting(E_ALL);



function afError($header, $text, $log=false) {
	global $db;

	if (!headers_sent()) header($header);

	echo $text;

	if ($log) _af_log_error($text, false);

	if (!empty($db)) $db->rollback();
	flush();
	die();
}



function error401($text='', $log=false) {
	global $afurl, $get;

	if ($get->server('HTTP_REFERER') === $afurl->all) {
		$afurl->redirect("$afurl->host$afurl->base/", 302);
	}

	afError(
		'HTTP/1.0 401 Authorization Required',
		"<div id=\"af-fatal\"><h1>ERROR: 401</h1>\n<h2>AUTHORIZATION REQUIRED</h2>\n<i>$text</i></div>\n",
		$log
	);
}



function error403($text='', $log=false) {
	afError(
		'HTTP/1.0 403 Forbidden',
		"<div id=\"af-fatal\"><h1>ERROR: 403</h1>\n<h2>FORBIDDEN</h2>\n<i>$text</i></div>\n",
		$log
	);
}



function error404($text='', $log=false) {
	global $afurl, $get, $site;

	if (!empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			$afurl->redirect("$afurl->host$afurl->base/", 302);
		}

		$text = htmlspecialchars($afurl->all) . '<br/>' . $text;

		if (!empty($site['debug'])) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
		}
	}

	afError(
		'HTTP/1.0 404 File Not Found',
		"<div id=\"af-fatal\"><h1>ERROR: 404</h1>\n<h2>FILE NOT FOUND</h2>\n<i>$text</i></div>\n",
		$log
	);
}



function error405($text='', $log=true) {
	afError(
		'HTTP/1.0 405 Method Not Allowed',
		"<div id=\"af-fatal\"><h1>ERROR: 405</h1>\n<h2>METHOD NOT ALLOWED</h2>\n<i>$text</i></div>\n",
		$log
	);
}



function error422($text='', $log=true) {
	afError(
		'HTTP/1.1 422 Unprocessable Entity',
		"<div id=\"af-fatal\"><h1>ERROR: 422</h1>\n<h2>UNPROCESSABLE ENTITY</h2>\n<i>$text</i></div>\n",
		$log
	);
}



function error500($text='', $log=true) {
	afError(
		'HTTP/1.0 500 Internal Server Error',
		"<div id=\"af-fatal\"><h1>ERROR: 500</h1>\n<h2>INTERNAL SERVER ERROR</h2>\n<i>$text</i></div>\n",
		$log
	);
}



function assert401($item, $text='', $log=false) {
	if (empty($item)) error401($text, $log);
	return $item;
}

function assert403($item, $text='', $log=false) {
	if (empty($item)) error403($text, $log);
	return $item;
}

function assert404($item, $text='', $log=false) {
	if (empty($item)) error404($text, $log);
	return $item;
}

function assert405($item, $text='', $log=false) {
	if (empty($item)) error405($text, $log);
	return $item;
}

function assert422($item, $text='', $log=true) {
	if (empty($item)) error422($text, $log);
	return $item;
}

function assert500($item, $text='', $log=true) {
	if (empty($item)) error500($text, $log);
	return $item;
}



function _af_log_error($data, $die=true) {
	global $site, $af, $afunit, $afurl, $db, $get, $user;

	//only print output once!
	static $echo = false;
	if ($echo) return;
	$echo = true;


	if (!isset($user)  ||  $user->hasPermission('admin')) {
		$site['debug'] = true;
	}

	$error = (ob_get_level()  &&  $die) ? htmlspecialchars(ob_get_clean()) : '';

	$arr = debug_backtrace(0);

	foreach ($arr as &$val) {
		foreach ($val as &$item) {
			if (is_array($item)  ||  is_object($item)) $item = [];
		} unset($item);
		$val = json_encode($val);
	} unset($val);

	if (!is_array($data)) $data = ['details'=>$data];

	$arr = array_merge(['time'=>@date('r')], $data, [
		'method'	=> !empty($get) ? $get->server('REQUEST_METHOD') : '',
		'url'		=> !empty($afurl->all)	? $afurl->all			: (!empty($get) ? $get->server('REQUEST_URI') : ''),
		'get'		=> !empty($_GET)		? json_encode($_GET)	: '',
		'post'		=> !empty($_POST)		? json_encode($_POST)	: '',
		'files'		=> !empty($_FILES)		? json_encode($_FILES)	: '',
		'user'		=> !empty($user)		? json_encode($user)	: '',
		'query'		=> !empty($db)			? $db->query()			: '',
		'mariadb'	=> !empty($db)			? $db->server()			: '',
		'server'	=> (!empty($get) ? $get->server('SERVER_ADDR') : '') . (!empty($afunit)?" : Unit $afunit":''),
		'remote'	=> !empty($get) ? $get->server('REMOTE_ADDR') : '',
		'agent'		=> !empty($get) ? $get->server('HTTP_USER_AGENT') : '',
		'referer'	=> !empty($get) ? $get->server('HTTP_REFERER') : '',
	], $arr);

	$out = str_replace("\n", "\r\n", print_r($arr, true)) . "\r\n";

	@file_put_contents(
		(isset($af)?$af->path():'') . '_log/' . @date('Y-m-d'),
		$out, FILE_APPEND
	);

	_af_email_error($out, !empty($arr['details']) ? $arr['details'] : '');

	if (!empty($db)) $db->rollback();

	if (!empty($site['debug'])  &&  $die) {
		error500("<pre style=\"text-align:left\">$out</pre>$error<br/><br/>");
	}

	if ($die) error500();
}



function _af_email_error($text, $title='') {
	global $site;

	if (empty($site['error_email'])) return;

	$tag = !empty($site['error_tag']) ? $site['error_tag'] : '';
	if (tbx_array($tag)) {
		$tag = '@'.implode("\r\n@", $tag);
	} else if (!empty($tag)) {
		$tag = '@' . $tag;
	}
	$tag = str_replace('@@', '@', trim($tag));

	@mail(
		$site['error_email'],
		date('r') . ' ' . strip_tags($title),
		$tag . "\r\n" . wordwrap($text, 75, "\r\n", true)
	);
}



set_error_handler(function($errno, $errstr, $errfile=NULL, $errline=NULL) {
	global $af;

	if (!error_reporting()) return false;

	$die = true;

	switch ($errno) {
		case E_WARNING:		case E_USER_WARNING:
		case E_NOTICE:		case E_USER_NOTICE:
			$die = (!empty($af) && is_object($af)) ? $af->debug() : false;
	}

	_af_log_error([
		'error'		=> $errno,
		'details'	=> $errstr,
		'file'		=> $errfile,
		'line'		=> $errline,
	], $die);

	return false;
});



set_exception_handler(function($e) {
	if (!error_reporting()) return false;

	_af_log_error([
		'error'		=> get_class($e),
		'details'	=> $e->getMessage(),
		'file'		=> $e->getFile(),
		'line'		=> $e->getLine(),
	]);
});



register_shutdown_function(function() {
	if (!error_reporting()) return;

	$e = error_get_last();

	if ($e['type'] !== E_ERROR  &&  $e['type'] !== E_PARSE) return;

	_af_log_error([
		'error'		=> $e['type'],
		'details'	=> $e['message'],
		'file'		=> $e['file'],
		'line'		=> $e['line'],
	]);
});



function _pudl_debug($callback, $db, $result=NULL) {
	_af_log_error([
		'error'		=> $db->errno(),
		'details'	=> $db->error(),
		'result'	=> ($result instanceof pudlResult) ? $result->error() : 'NULL',
	]);
}
