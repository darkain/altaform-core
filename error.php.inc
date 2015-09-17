<?php


function afError($header, $text) {
	global $site, $db;

	if (!headers_sent()) header($header);

	if (!empty($site['debug'])  &&  !empty($db)) {
		$text .= htmlspecialchars($db->query());
		_af_log_error($text, false);
	}

	if (!empty($db)) $db->rollback();

	echo $text;
	flush();
	die();
}


function error401($text='') {
	global $afurl, $get, $af;

	if ($get->server('HTTP_REFERER') === $afurl->all) {
		$af->redirect("$afurl->host$afurl->base/", 302);
	}

	afError(
		'HTTP/1.0 401 Authorization Required',
		"<h1>ERROR: 401</h1>\n<h2>AUTHORIZATION REQUIRED</h2>\n<i>$text</i><br/>\n"
	);
}


function error403($text='') {
	afError(
		'HTTP/1.0 403 Forbidden',
		"<h1>ERROR: 403</h1>\n<h2>FORBIDDEN</h2>\n<i>$text</i><br/>\n"
	);
}


function error404($text='') {
	global $afurl, $get, $af, $site;

	if (!empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			$af->redirect("$afurl->host$afurl->base/", 302);
		}

		$text = htmlspecialchars($afurl->all) . '<br/>' . $text;

		if (!empty($site['debug'])) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
		}
	}

	afError(
		'HTTP/1.0 404 File Not Found',
		"<h1>ERROR: 404</h1>\n<h2>FILE NOT FOUND</h2>\n<i>$text</i><br/>\n"
	);
}


function error422($text='') {
	afError(
		'HTTP/1.1 422 Unprocessable Entity',
		"<h1>ERROR: 422</h1>\n<h2>UNPROCESSABLE ENTITY</h2>\n<i>$text</i><br/>\n"
	);
}


function error500($text='') {
	afError(
		'HTTP/1.0 500 Internal Server Error',
		"<h1>ERROR: 500</h1>\n<h2>INTERNAL SERVER ERROR</h2>\n<i>$text</i><br/>\n"
	);
}



function assert401($item, $text='') { if (empty($item)) error401($text); }
function assert403($item, $text='') { if (empty($item)) error403($text); }
function assert404($item, $text='') { if (empty($item)) error404($text); }
function assert422($item, $text='') { if (empty($item)) error422($text); }
function assert500($item, $text='') { if (empty($item)) error500($text); }




function _af_log_error($text, $die=true) {
	global $site, $af, $db;

	//only print output once!
	static $echo = false;
	if ($echo) return;
	$echo = true;


	if (!isset($af)) {
		$site['debug'] = true;
	} else if ($af->hasPermission('admin')) {
		$site['debug'] = true;
	}

	$error = '';
	if (ob_get_level()  &&  $die) $error = htmlspecialchars(ob_get_clean());

	$arr = debug_backtrace();

	foreach ($arr as &$val) {
		foreach ($val as &$item) {
			if (is_array($item)  ||  is_object($item)) $item = array();
		} unset($item);
		$val = json_encode($val);
	} unset($val);

	if (is_array($text)) {
		$arr = array_merge($text, $arr);
	} else {
		$arr[0] = $text;
	}

	$out = print_r($arr, true) . "\n";

	@file_put_contents(
		(isset($af)?$af->path():'') . '_log/' . @date('Y-m-d'),
		$out,
		FILE_APPEND
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
	if (is_array($tag)) {
		$tag = '@'.implode("\r\n@", $tag);
	} else if (!empty($tag)) {
		$tag = '@' . $tag;
	}
	$tag = str_replace('@@', '@', trim($tag));

	@mail(
		$site['error_email'],
		date('r') . ' ' . $title,
		$tag . "\r\n" . $text
	);
}




function _af_error_handler($errno, $errstr, $errfile=NULL, $errline=NULL, $errcontext=NULL) {
	global $af, $db, $get, $afurl;

	if (!error_reporting()) return false;

	$die = true;

	switch ($errno) {
		case E_WARNING:		case E_USER_WARNING:
		case E_NOTICE:		case E_USER_NOTICE:
			$die = (!empty($af) && is_object($af)) ? $af->debug() : false;
	}

	_af_log_error(array(
		'time'		=> @date('r'),
		'error'		=> $errno,
		'details'	=> $errstr,
		'file'		=> $errfile,
		'line'		=> $errline,
		'url'		=> isset($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'),
		'mariadb'	=> !empty($db) ? $db->server() : '',
		'proxy'		=> $get->server('SERVER_ADDR'),
		'server'	=> $get->server('X-PHP-IP'),
		'remote'	=> $get->server('REMOTE_ADDR'),
		'agent'		=> $get->server('HTTP_USER_AGENT'),
	), $die);

	return false;
}
set_error_handler('_af_error_handler');




function _af_log_exception(Exception $e) {
	global $db, $get, $afurl;

	if (!error_reporting()) return false;

	_af_log_error(array(
		'time'		=> @date('r'),
		'error'		=> get_class($e),
		'details'	=> $e->getMessage(),
		'file'		=> $e->getFile(),
		'line'		=> $e->getLine(),
		'url'		=> isset($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'),
		'mariadb'	=> !empty($db) ? $db->server() : '',
		'proxy'		=> $get->server('SERVER_ADDR'),
		'server'	=> $get->server('X-PHP-IP'),
		'remote'	=> $get->server('REMOTE_ADDR'),
		'agent'		=> $get->server('HTTP_USER_AGENT'),
	));
}
set_exception_handler('_af_log_exception');




function _af_fatal_error() {
	$e = error_get_last();
	if ($e['type'] !== E_ERROR  &&  $e['type'] !== E_PARSE ) return;
	_af_error_handler($e['type'], $e['message'], $e['file'], $e['line']);
}
register_shutdown_function('_af_fatal_error');




function _pudl_debug($db, $result=false) {
	global $get, $afurl;

	_af_log_error(array(
		'time'		=> @date('r'),
		'error'		=> $db->errno(),
		'details'	=> $db->error(),
		'query'		=> ($result !== false) ? ($result->query()) : ($db->query()),
		'url'		=> isset($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'),
		'mariadb'	=> !empty($db) ? $db->server() : '',
		'proxy'		=> $get->server('SERVER_ADDR'),
		'server'	=> $get->server('X-PHP-IP'),
		'remote'	=> $get->server('REMOTE_ADDR'),
		'agent'		=> $get->server('HTTP_USER_AGENT'),
	));
}
