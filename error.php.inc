<?php


class afException extends Exception {}



class aferror {


	public static function render($header, $text, $log=false, $template=false) {
		global $db, $af, $title;

		if (!headers_sent()) header($header);

		$title = str_replace('HTTP/1.1 ', '', $header);

		$data = $log ? self::log($text, false) : self::process($text);

		if (!empty($template)  &&  $af instanceof altaform  &&  !empty($af->config->root)) {
			if (is_file($af->path().$af->config->root.'/'.$template)) {
				$ok = self::renderFile(
					$af->path().$af->config->root.'/'.$template,
					$header, $text, $data
				);
			}
		}

		if (empty($ok)  &&  $af instanceof altaform  &&  !empty($af->config->root)) {
			if (is_file($af->path().$af->config->root.'/error.tpl')) {
				$ok = self::renderFile(
					$af->path().$af->config->root.'/error.tpl',
					$header, $text, $data
				);
			}
		}

		if (empty($ok)) {
			echo is_array($text) ? implode("\n", $text) : $text;
		}

		if ($db instanceof pudl) $db->rollback();
		flush();
		die();
	}




	public static function renderFile($file, $header, $text, $data) {
		global $af;

		return $af->resetStage()->renderPage($file, [
			'error' => [
				'header'	=> $header,
				'text'		=> $text,
				'details'	=> $data,
			]
		]);
	}




	public static function process($data) {
		global $af, $afdevice, $afurl, $db, $get, $user;

		if (!is_array($data)) $data = ['details'=>$data];

		foreach ($data as $key => &$val) {
			if (!is_int($key)) continue;
			if (is_array($val)) foreach ($val as &$item) {
				if (is_array($item)  ||  is_object($item)) $item = [];
			} unset($item);
			$val = json_encode($val, JSON_UNESCAPED_SLASHES);
		} unset($val);

		if (!empty($get)) {
			$address = (empty($get) ? '' :
				($get->server('REQUEST_METHOD') . ' ' . $get->server('SERVER_PROTOCOL') . ' '))
				. (!empty($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'));
		} else {
			$address = !empty($afurl->all) ? $afurl->all : '';
		}

		$return = array_merge($data, [
			'address'		=> !empty($data['address']) ? $data['address'] : $address,
			'error-time'	=> @date('r'),
			'error-path'	=> getcwd(),
			'var-get'		=> !empty($_GET)	? json_encode($_GET,			JSON_UNESCAPED_SLASHES) : '',
			'var-post'		=> !empty($_POST)	? json_encode($_POST,			JSON_UNESCAPED_SLASHES) : '',
			'var-files'		=> !empty($_FILES)	? json_encode($_FILES,			JSON_UNESCAPED_SLASHES) : '',
			'var-user'		=> !empty($user)	? json_encode($user,			JSON_UNESCAPED_SLASHES) : '',
			'template'		=> !empty($af)		? json_encode($af->filepath,	JSON_UNESCAPED_SLASHES) : '',
			'query'			=> !empty($db)		? $db->query()						: '',
			'ip-database'	=> !empty($db)		? $db->server()						: '',
			'ip-php'		=> !empty($get)		? $get->server('X-PHP-IP')			: '',
			'ip-httpd'		=> !empty($get)		? $get->server('SERVER_ADDR')		: '',
			'ip-client'		=> !empty($get)		? $get->server('REMOTE_ADDR')		: '',
			'user-agent'	=> !empty($get)		? $get->server('HTTP_USER_AGENT')	: '',
			'user-referer'	=> !empty($get)		? $get->server('HTTP_REFERER')		: '',
			'user-device'	=> !empty($afdevice)? $afdevice() : '',
		]);

		if (!empty($afurl->redirected)) {
			$return['redirected'] = json_encode(
				$afurl->redirected,
				JSON_UNESCAPED_SLASHES
			);
		}

		ksort($return, SORT_NATURAL);
		return $return;
	}




	public static function log($data, $die=true, $backtrace=[]) {
		global $af, $db, $user, $afconfig;

		//ONLY LOG ERROR ONCE!
		static $echo = false;
		if ($echo) return false;
		$echo = true;

		if (empty($backtrace)) $backtrace = debug_backtrace(0);

		if (!($user instanceof afuser)  ||  $user->isAdmin()) {
			$afconfig->debug = true;
		}

		$error = (ob_get_level()  &&  $die)
			? htmlspecialchars(ob_get_clean(),TBX_SPECIAL_CHARS)
			: '';

		$arr = $arrout = self::process($data);

		$arrout += $backtrace;
		foreach ($arrout as $key => &$value) if (is_int($key)) {
			$value = json_encode($value, JSON_UNESCAPED_SLASHES);
		}

		$out = str_replace("\n", "\r\n", print_r($arrout, true)) . "\r\n";

		@file_put_contents(
			($af instanceof altaform ? $af->path() : '') . '_log/' . @date('Y-m-d'),
			$out, FILE_APPEND
		);

		self::email($out, !empty($arr['details']) ? $arr['details'] : '');

		if ($db instanceof pudl) $db->rollback();

		if (!$die) return $arr;


		if (empty($afconfig->debug)) return error500('', true, $arr);


		$html = '<table class="af-debug-backtrace">';

		foreach ($backtrace as $key => $value) {
			$html .= '<tr><td>' . ($key+1) . '</td><td class="nobr">Line: ';
			$html .= htmlspecialchars(
				array_key_exists('line', $value) ? $value['line'] : '?',
				TBX_SPECIAL_CHARS
			);
			$html .= '</td><td>File: ';
			$html .= htmlspecialchars(
				array_key_exists('file', $value) ? $value['file'] : '?',
				TBX_SPECIAL_CHARS
			);
			if (!empty($value['function'])) {
				$html .= '<br/>' . $value['function'] . '(';
				if (!empty($value['args'])) {
					$html .= substr(
						htmlspecialchars(
							json_encode($value['args'], JSON_UNESCAPED_SLASHES),
							TBX_SPECIAL_CHARS
						),
						1, -1
					);
				}
				$html .= ')';
			}
			$html .= '</td></tr>';
		}

		foreach ($arr as $key => $value) {
			$html .= '<tr><td colspan="2" class="nobr">';
			$html .= htmlspecialchars($key,TBX_SPECIAL_CHARS);
			$html .= '</td><td>';
			$html .= htmlspecialchars($value,TBX_SPECIAL_CHARS);
			$html .= '</td></tr>';
		}

		$html .= '</table>';

		error500($html, true, $arr);
	}




	public static function email($text, $title='') {
		global $afconfig;

		if (empty($afconfig->error['email'])) return;

		$tag = !empty($afconfig->error['tag']) ? $afconfig->error['tag'] : '';
		if (tbx_array($tag)) {
			$tag = '@'.implode("\r\n@", $tag);
		} else if (!empty($tag)) {
			$tag = '@' . $tag;
		}
		$tag = str_replace('@@', '@', trim($tag));

		@mail(
			$afconfig->error['email'],
			date('r') . ' ' . strip_tags($title),
			$tag . "\r\n" . wordwrap($text, 75, "\r\n", true)
		);
	}




	public static function javascript() {
		global $get;

		if ($get('message') === 'Script error.') return;

		if (preg_match('/bot|crawl|slurp|spider/i', $get->server('HTTP_USER_AGENT'))) {
			return;
		}

		self::log([
			'address'		=> $get('url'),
			'details'		=> 'JavaScript: ' . $get('message'),
			'error-file'	=> $get('file'),
			'error-line'	=> $get('line') . ':' . $get('col'),
		], false);
	}
}




function error401($text='', $log=false, $details=false) {
	aferror::render('HTTP/1.1 401 Unauthorized', [
		'<div id="af-fatal"><h1>ERROR: 401</h1>',
		'<h2>AUTHORIZATION REQUIRED</h2>',
		'<h3>' . htmlspecialchars($details,TBX_SPECIAL_CHARS) . '</h3>',
		'<i>' . $text . '</i></div>',
	], $log, 'error401.tpl');
}



function error403($text='', $log=false, $details=false) {
	aferror::render('HTTP/1.1 403 Forbidden', [
		'<div id="af-fatal"><h1>ERROR: 403</h1>',
		'<h2>FORBIDDEN</h2>',
		'<h3>' . htmlspecialchars($details,TBX_SPECIAL_CHARS) . '</h3>',
		'<i>' . $text . '</i></div>',
	], $log, 'error403.tpl');
}



function error404($text='', $log=false, $details=false) {
	global $afurl, $get, $afconfig;

	if (!empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			$afurl->redirect("$afurl->host$afurl->base/", 302);
		}

		$text = htmlspecialchars($afurl->all,TBX_SPECIAL_CHARS) . '<br/>' . $text;

		if (!empty($afconfig->debug)) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
		}
	}

	aferror::render('HTTP/1.1 404 File Not Found', [
		'<div id="af-fatal"><h1>ERROR: 404</h1>',
		'<h2>FILE NOT FOUND</h2>',
		'<h3>' . htmlspecialchars($details,TBX_SPECIAL_CHARS) . '</h3>',
		'<i>' . $text . '</i></div>',
	], $log, 'error404.tpl');
}



function error405($text='', $log=true, $details=false) {
	aferror::render('HTTP/1.1 405 Method Not Allowed', [
		'<div id="af-fatal"><h1>ERROR: 405</h1>',
		'<h2>METHOD NOT ALLOWED</h2>',
		'<h3>' . htmlspecialchars($details,TBX_SPECIAL_CHARS) . '</h3>',
		'<i>' . $text . '</i></div>',
	], $log, 'error405.tpl');
}



function error422($text='', $log=true, $details=false) {
	aferror::render('HTTP/1.1 422 Unprocessable Entity', [
		'<div id="af-fatal"><h1>ERROR: 422</h1>',
		'<h2>UNPROCESSABLE ENTITY</h2>',
		'<h3>' . htmlspecialchars($details,TBX_SPECIAL_CHARS) . '</h3>',
		'<i>' . $text . '</i></div>',
	], $log, 'error422.tpl');
}



function error500($text='', $log=true, $details=false) {
	if (is_array($details)) $details = $details['details'];

	aferror::render('HTTP/1.1 500 Internal Server Error', [
		'<div id="af-fatal"><h1>ERROR: 500</h1>',
		'<h2>INTERNAL SERVER ERROR</h2>',
		'<h3>' . htmlspecialchars($details,TBX_SPECIAL_CHARS) . '</h3>',
		'<i>' . $text . '</i></div>',
	], $log, 'error500.tpl');
}



function assert401($item, $text='', $log=false) {
	return (empty($item) && $item!=='') ? error401($text, $log) : $item;
}

function assert403($item, $text='', $log=false) {
	return (empty($item) && $item!=='') ? error403($text, $log) : $item;
}

function assert404($item, $text='', $log=false) {
	return (empty($item) && $item!=='') ? error404($text, $log) : $item;
}

function assert405($item, $text='', $log=false) {
	return (empty($item) && $item!=='') ? error405($text, $log) : $item;
}

function assert422($item, $text='', $log=true) {
	return (empty($item) && $item!=='') ? error422($text, $log) : $item;
}

function assert500($item, $text='', $log=true) {
	return (empty($item) && $item!=='') ? error500($text, $log) : $item;
}

function assertRead($item, $text='', $log=false) {
	return ($item !== true && !in_array($item, ['read', 'write', 'grant']))
		? error401($text, $log) : $item;
}

function assertWrite($item, $text='', $log=false) {
	return ($item !== true && !in_array($item, ['write', 'grant']))
		? error401($text, $log) : $item;
}

function assertGrant($item, $text='', $log=false) {
	return ($item !== true && !in_array($item, ['grant']))
		? error401($text, $log) : $item;
}



set_error_handler(function($errno, $errstr, $errfile=NULL, $errline=NULL, $errcontext=[], $backtrace=[]) {
	global $afconfig;

	if (!error_reporting()) return false;

	$die = true;

	switch ($errno) {
		case E_WARNING:		case E_USER_WARNING:
		case E_NOTICE:		case E_USER_NOTICE:
			$die = ($afconfig instanceof af_config)
				? !!$afconfig->debug
				: false;
	}

	aferror::log([
		'error-code'	=> $errno,
		'details'		=> $errstr,
		'error-file'	=> $errfile,
		'error-line'	=> $errline,
	], $die, $backtrace);

	return true;
});



set_exception_handler(function($e) {
	if (!error_reporting()) return false;

	aferror::log([
		'error-code'	=> get_class($e) . ':' . $e->getCode(),
		'details'		=> $e->getMessage(),
		'error-file'	=> $e->getFile(),
		'error-line'	=> $e->getLine(),
	], true, $e->getTrace());
});



register_shutdown_function(function() {
	if (!error_reporting()) return;

	$e = error_get_last();

	if ($e['type'] !== E_ERROR  &&  $e['type'] !== E_PARSE) return;

	aferror::log([
		'error-code'	=> $e['type'],
		'details'		=> $e['message'],
		'error-file'	=> $e['file'],
		'error-line'	=> $e['line'],
	]);
});



function _pudl_log($callback, $db, $result=NULL) {
	global $af;

	$path = $af instanceof altaform ? $af->path() : '';

	@file_put_contents(
		$path . '_log/' . @date('Y-m-d') . '-query',
		$db->query() . "\n",
		FILE_APPEND
	);
}



function _pudl_debug($callback, $db, $result=NULL) {
	aferror::log([
		'error-code'	=> $db->errno(),
		'details'		=> $db->error(),
		'result'		=> ($result instanceof pudlResult) ? $result->error() : 'NULL',
	]);
}
