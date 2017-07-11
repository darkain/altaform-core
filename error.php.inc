<?php


class afException extends Exception {}



class aferror {


	public static function render($header, $text, $log=false, $template=false) {
		global $db, $af;

		if (!headers_sent()) {
			$protocol	= !empty($_SERVER['SERVER_PROTOCOL'])
						? $_SERVER['SERVER_PROTOCOL']
						: 'HTTP/1.1';
			header($protocol . ' ' . $header);
		}

		if ($af instanceof altaform) {
			$af->title = $header;
		}

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
			if ($af instanceof altaform  &&  $af->cli()) {
				if (is_array($text)) $text = implode("\n", $text);

				$text = str_replace(
					['&lt;br /&gt;', '&lt;br/&gt;', '&lt;br&gt;', '<br/>', '<br />'],
					"\n", $text
				);

				echo strip_tags(str_replace(
					["\r\n",	'&apos;',	'&amp;',	'</td><td>'],
					["\n",		"'",		'&',		":\t"],
					$text
				));

			} else {
				echo str_replace(
					['&lt;br /&gt;', '&lt;br/&gt;', '&lt;br&gt;'],
					'<br />',
					is_array($text) ? implode("\n", $text) : $text
				);
			}
		}

		if ($db instanceof pudl) $db->rollback();
		echo "\n";
		flush();
		die();
	}




	public static function renderFile($file, $header, $text, $data) {
		global $af;

		if ($af->contentType() !== 'html') return false;
		if ($af->stage() === AF_STAGE_HEADER) return false;

		return $af->resetStage()->renderPage($file, [
			'error' => [
				'header'	=> $header,
				'text'		=> $text,
				'details'	=> $data,
			]
		]);
	}




	public static function process($data) {
		global $af, $afurl, $db, $get, $user;

		if (!is_array($data)) $data = ['details'=>$data];

		foreach ($data as $key => &$val) {
			if (!is_int($key)) continue;
			if (is_array($val)) foreach ($val as &$item) {
				if (is_array($item)  ||  is_object($item)) $item = [];
			} unset($item);
			$val = self::json($val);
		} unset($val);

		if (!empty($get)) {
			$address = (empty($get) ? '' :
				($get->server('REQUEST_METHOD') . ' ' . $get->server('SERVER_PROTOCOL') . ' '))
				. (!empty($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'));
		} else {
			$address = !empty($afurl->all) ? $afurl->all : '';
		}

		$return = array_merge($data, [
			'address'		=> empty($data['address']) ? $address : $data['address'],
			'error-time'	=> @date('r'),
			'error-path'	=> getcwd(),
			'var-get'		=> !isset($_GET)		? '' :	self::json($_GET),
			'var-post'		=> !isset($_POST)		? '' :	self::json($_POST),
			'var-files'		=> !isset($_FILES)		? '' :	self::json($_FILES),
			'var-user'		=> !isset($user)		? '' :	self::json($user->raw()),
			'template'		=> !isset($af)			? '' :	self::json($af->filepath),
			'query'			=> !isset($db)			? '' :	$db->query(),
			'ip-database'	=> !isset($db)			? '' :	$db->server(),
			'ip-php'		=> !isset($get)			? '' :	$get->server('X-PHP-IP'),
			'ip-httpd'		=> !isset($get)			? '' :	$get->server('SERVER_ADDR'),
			'ip-client'		=> !isset($get)			? '' :	$get->server('REMOTE_ADDR'),
			'user-agent'	=> !isset($get)			? '' :	$get->server('HTTP_USER_AGENT'),
			'user-referer'	=> !isset($get)			? '' :	$get->server('HTTP_REFERER'),
			'user-device'	=> !isset($af->device)	? '' :	$af->device(),
		]);

		if (!empty($afurl->redirected)) {
			$return['redirected'] = self::json($afurl->redirected);
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

		if (!($user instanceof af_user)  ||  $user->isAdmin()) {
			if (empty($afconfig)) $afconfig = (object)[];
			$afconfig->debug = true;
		}

		$error = (ob_get_level()  &&  $die) ? self::html(ob_get_clean()) : '';

		$arr = $arrout = self::process($data);

		$arrout += $backtrace;
		foreach ($arrout as $key => &$value) if (is_int($key)) {
			if (!empty($value['args'])  &&  is_array($value['args'])) {
				@array_walk_recursive($value['args'], [__CLASS__,'nopassword']);
			}
			$value = self::json($value);
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


		$html = "\n" . '<table class="af-debug-backtrace">' . "\n";

		foreach ($backtrace as $key => $value) {
			$html .= '<tr><td>' . ($key+1) . '</td><td class="nobr">Line: ';
			$html .= self::html(array_key_exists('line', $value) ? $value['line'] : '?');
			$html .= '</td><td>File: ';
			$html .= self::html(array_key_exists('file', $value) ? $value['file'] : '?');
			if (!empty($value['function'])) {
				$html .= '<br/>' . $value['function'] . '(';
				if (!empty($value['args'])) {
					@array_walk_recursive($value['args'], [__CLASS__,'nopassword']);
					$html .= substr(self::html_json($value['args']), 1, -1);
				}
				$html .= ')';
			}
			$html .= "</td></tr>\n";
		}

		foreach ($arr as $key => $value) {
			$html .= '<tr><td colspan="2" class="nobr">';
			$html .= self::html($key);
			$html .= '</td><td>';
			$html .= self::html($value);
			$html .= "</td></tr>\n";
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

		// GENERIC ERROR MESSAGE IN JS FILE NOT HOSTED BY US (CROSS-SITE)
		if ($get('message') === 'Script error') return;
		if ($get('message') === 'Script error.') return;

		// https://github.com/getsentry/raven-js/issues/756
		if (strpos($get('message'), "evaluating 'elt.parentNode'")) return;

		// IGNORE ISSUES WITH CriOS INSERTED JAVASCRIPT FILE
		if (strpos($get('message'), '__gCrWeb')) return;

		// IGNORE ISSUES WITH "MULTIFACTOR AUTHENTICATION CLIENT"
		if (strpos($get('message'), 'fidoCallback')) return;

		// IGNORE ISSUES WITH "BLINGBAR" TOOLBAR
		if (strpos($get('message'), 'BLNGBAR')) return;

		// IGNORE JQUERY UI ERRORS - THIS LIBRARY IS RANDOMLY BUGGY
		if (strpos($get('file'), 'jquery-ui.min.js')) return;

		// IGNORE ERRORS FROM BOTS. THEY ARE TERRIBAD AT PROCESSING SCRIPTS PROPERLY
		if (preg_match(
			'/bot|crawl|slurp|spider|ucbrowser|wkhtmltopdf/i',
			$get->server('HTTP_USER_AGENT'))) {
			return;
		}

		self::log([
			'address'		=> $get('url'),
			'details'		=> 'JavaScript: ' . $get('message'),
			'error-file'	=> $get('file'),
			'error-line'	=> $get('line') . ':' . $get('col'),
		], false);
	}




	public static function html($item) {
		if (!defined('TBX_SPECIAL_CHARS')) {
			define('TBX_SPECIAL_CHARS', ENT_NOQUOTES|ENT_HTML5);
		}
		return htmlspecialchars((string)$item, TBX_SPECIAL_CHARS);
	}



	public static function json($item) {
		return @json_encode($item,
			JSON_UNESCAPED_SLASHES |
			JSON_UNESCAPED_UNICODE |
			JSON_PARTIAL_OUTPUT_ON_ERROR
		);
	}



	public static function html_json($item) {
		return self::html(self::json($item));
	}



	public static function nopassword(&$item, $key) {
		if (is_int($key)) return;

		switch ($key) {
			case 'pass':
			case 'password':
			case 'hash':
			case 'key':
			case 'secure':
				$item = '********';
		}
	}
}




function error401($text=false, $log=false, $details=false) {
	aferror::render('401 Unauthorized', [
		'<div id="af-fatal"><h1>ERROR: 401</h1>',
		'<h2>AUTHORIZATION REQUIRED</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error401.tpl');
}




function error402($text=false, $log=false, $details=false) {
	aferror::render('402 Payment Required', [
		'<div id="af-fatal"><h1>ERROR: 402</h1>',
		'<h2>PAYMENT REQUIRED</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error402.tpl');
}




function error403($text=false, $log=false, $details=false) {
	aferror::render('403 Forbidden', [
		'<div id="af-fatal"><h1>ERROR: 403</h1>',
		'<h2>FORBIDDEN</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error403.tpl');
}




function error404($text=false, $log=false, $details=false) {
	global $afurl, $get, $afconfig;

	if (!empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			$afurl->redirect("$afurl->host$afurl->base/", 302);
		}

		$text = aferror::html($afurl->all) . '<br/>' . $text;

		if (!empty($afconfig->debug)) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
		}
	}

	aferror::render('404 File Not Found', [
		'<div id="af-fatal"><h1>ERROR: 404</h1>',
		'<h2>FILE NOT FOUND</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error404.tpl');
}




function error405($text=false, $log=true, $details=false) {
	aferror::render('405 Method Not Allowed', [
		'<div id="af-fatal"><h1>ERROR: 405</h1>',
		'<h2>METHOD NOT ALLOWED</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error405.tpl');
}




function error422($text=false, $log=true, $details=false) {
	aferror::render('422 Unprocessable Entity', [
		'<div id="af-fatal"><h1>ERROR: 422</h1>',
		'<h2>UNPROCESSABLE ENTITY</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error422.tpl');
}




function error500($text=false, $log=true, $details=false) {
	if (is_array($details)) $details = $details['details'];

	aferror::render('500 Internal Server Error', [
		'<div id="af-fatal"><h1>ERROR: 500</h1>',
		'<h2>INTERNAL SERVER ERROR</h2>',
		'<h3>' . aferror::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error500.tpl');
}




function assert401($item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assert401($text);
	return (empty($item) && $item!=='') ? error401($text, $log) : $item;
}


function assert402($item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assert402($text);
	return (empty($item) && $item!=='') ? error402($text, $log) : $item;
}


function assert403($item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assert403($text);
	return (empty($item) && $item!=='') ? error403($text, $log) : $item;
}


function assert404($item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assert404($text);
	return (empty($item) && $item!=='') ? error404($text, $log) : $item;
}


function assert405($item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assert405($text);
	return (empty($item) && $item!=='') ? error405($text, $log) : $item;
}


function assert422($item, $text=false, $log=true) {
	if ($item instanceof pudlOrm) return $item->assert422($text);
	return (empty($item) && $item!=='') ? error422($text, $log) : $item;
}


function assert500($item, $text=false, $log=true) {
	if ($item instanceof pudlOrm) return $item->assert500($text);
	return (empty($item) && $item!=='') ? error500($text, $log) : $item;
}


function assertRead($item, $text=false, $log=false) {
	return ($item !== true && !in_array($item, ['read', 'write', 'grant']))
		? error401($text, $log) : $item;
}


function assertWrite($item, $text=false, $log=false) {
	return ($item !== true && !in_array($item, ['write', 'grant']))
		? error401($text, $log) : $item;
}


function assertGrant($item, $text=false, $log=false) {
	return ($item !== true && !in_array($item, ['grant']))
		? error401($text, $log) : $item;
}




set_error_handler(function($errno, $errstr, $errfile=NULL, $errline=NULL, $errcontext=[], $backtrace=[]) {
	global $afconfig;

	if (!(error_reporting()  &  $errno)) return false;

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

	return $die;
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
