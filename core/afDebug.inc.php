<?php

require_once(is_owner(__DIR__.'/../modules/status.php'));







class afDebug {


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

		$data = $log ? static::log($text, false) : static::process($text);

		if (!empty($template)  &&  $af instanceof altaform  &&  !empty($af->config->root)) {
			if (is_file($af->path().$af->config->root.'/'.$template)) {
				$ok = static::renderFile(
					$af->path().$af->config->root.'/'.$template,
					$header, $text, $data
				);
			}
		}

		if (empty($ok)  &&  $af instanceof altaform  &&  !empty($af->config->root)) {
			if (is_file($af->path().$af->config->root.'/error.tpl')) {
				$ok = static::renderFile(
					$af->path().$af->config->root.'/error.tpl',
					$header, $text, $data
				);
			}
		}

		if (empty($ok)) {
			if (function_exists('\af\cli') && \af\cli()) {
				if (is_array($text)) $text = implode("\n", $text);

				$text = str_replace(
					['&lt;br /&gt;', '&lt;br/&gt;', '&lt;br&gt;', '<br/>', '<br />'],
					"\n", $text
				);

				echo strip_tags(str_replace(
					["\r\n",	'&apos;',	'&amp;',	'</th><th>',	'</th><td>'],
					["\n",		"'",		'&',		":\t",			":\t"],
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
		exit(1);
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
		global $af, $afurl, $router, $db, $get, $user;

		if (!is_array($data)) $data = ['details'=>$data];

		foreach ($data as $key => &$val) {
			if (!is_int($key)) continue;
			if (is_array($val)) foreach ($val as &$item) {
				if (is_array($item)  ||  is_object($item)) $item = [];
			} unset($item);
			$val = static::json($val);
		} unset($val);

		if (!empty($get)) {
			$address = (empty($get) ? '' :
				($get->server('REQUEST_METHOD') . ' ' . $get->server('SERVER_PROTOCOL') . ' '))
				. (!empty($afurl->all) ? $afurl->all : $get->server('REQUEST_URI'));
		} else {
			$address = !empty($afurl->all) ? $afurl->all : '';
		}

		if ($user instanceof pudlObject) {
			$userdata = new pudlObject;
			$userdata->merge($user, false);
			$userdata = $userdata->raw();
		} else {
			$userdata = [];
		}

		$return = array_merge([
			'error-time'	=> @date('r'),
			'error-path'	=> getcwd(),

			'var-user'		=> static::json($userdata),
			'var-get'		=> !isset($_GET)	? '' :	static::json($_GET),
			'var-post'		=> !isset($_POST)	? '' :	static::json($_POST),
			'var-files'		=> !isset($_FILES)	? '' :	static::json($_FILES),
			'af-template'	=> !isset($af)		? '' :	static::json($af->filepath),
			'db-query'		=> !isset($db)		? '' :	$db->query(),
			'ip-database'	=> !isset($db)		? '' :	$db->server(),
			'user-agent'	=> !isset($get)		? '' :	$get->server('HTTP_USER_AGENT'),
			'user-referer'	=> !isset($get)		? '' :	$get->server('HTTP_REFERER'),

			'af-url'		=> empty($data['address'])	? $address : $data['address'],
			'user-device'	=> !isset($af->device)		? '' :	$af->device(),
			'ip-php'		=> !class_exists('\af\ip')	? '' :	\af\ip::local(),
			'ip-client'		=> !class_exists('\af\ip')	? '' :	\af\ip::address(),
			'ip-httpd'		=> !class_exists('\af\ip')	? '' :	\af\ip::server(),
		], $data);

		if (!empty($router->redirected)) {
			$return['redirected'] = static::json($router->redirected, false);
		}

		ksort($return, SORT_NATURAL);
		return $return;
	}




	public static function log($data, $end=true, $backtrace=[]) {
		global $af, $db, $user, $afconfig;

		if (class_exists('altaform')) altaform::$error = true;

		//ONLY LOG ERROR ONCE!
		static $echo = false;
		if ($echo) return false;
		$echo = true;

		//DISABLE RECURSIVE ERROR REPORTING
		$olderr = error_reporting(0);

		if (empty($backtrace)) $backtrace = debug_backtrace(0);

		if (!($user instanceof afUser)  ||  $user->isAdmin()) {
			if (empty($afconfig)) {
				$afconfig = (object)[];
				$afconfig->debug = true;
			}
		}

		$error = (ob_get_level()  &&  $end) ? static::html(ob_get_clean()) : '';

		$arr = $arrout = static::process($data);

		$arrout += $backtrace;
		foreach ($arrout as $key => &$value) if (is_int($key)) {
			if (!empty($value['args'])  &&  is_array($value['args'])) {
				@array_walk_recursive($value['args'], [__CLASS__,'nopassword']);
			}
			$value = static::json($value);
		}

		$out = str_replace("\n", "\r\n", print_r($arrout, true)) . "\r\n";

		@file_put_contents(
			($af instanceof altaform ? $af->path() : '') . '_log/' . @date('Y-m-d'),
			$out, FILE_APPEND
		);

		static::email($out, !empty($arr['details']) ? $arr['details'] : '');

		//TODO: $db SHOULD BE PASSED IN, RATHER THAN GLOBAL
		if ($db instanceof pudl) $db->rollback();


		if (!$end) {
			error_reporting($olderr);
			return $arr;
		}


		if (empty($afconfig->debug)) {
			return \af\error(500, '', true, $arr);
		}


		$html = "\n" . '<table class="af-debug-backtrace">' . "\n";

		foreach ($backtrace as $key => $value) {
			$html .= '<tr><th>' . ($key+1) . '</th><th>Line: ';
			$html .= static::html(array_key_exists('line', $value) ? $value['line'] : '?');
			$html .= '</th><td>File: ';
			$html .= static::html(array_key_exists('file', $value) ? $value['file'] : '?');
			if (!empty($value['function'])) {
				$html .= '<br/>' . $value['function'] . '(';
				if (!empty($value['args'])) {
					@array_walk_recursive($value['args'], [__CLASS__,'nopassword']);
					$html .= substr(static::html_json($value['args']), 1, -1);
				}
				$html .= ')';
			}
			$html .= "</td></tr>\n";
		}

		foreach ($arr as $key => $value) {
			$html .= '<tr><th colspan="2">';
			$html .= static::html($key);
			$html .= '</th><td>';
			$html .= static::html($value);
			$html .= "</td></tr>\n";
		}

		$html .= '</table>';

		\af\error(500, $html, true, $arr);
	}




	public static function email($text, $title='') {
		global $afconfig, $afurl;

		if (empty($afconfig->error['email'])) return;

		$tag = !empty($afconfig->error['tag']) ? $afconfig->error['tag'] : '';
		if (tbx_array($tag)) {
			$tag = '@'.implode("\r\n@", $tag);
		} else if (!empty($tag)) {
			$tag = '@' . $tag;
		}
		$tag = str_replace('@@', '@', trim($tag));

		return @mail(
			$afconfig->error['email'],
			date('r') . ' ' . strip_tags($title),
			$tag . "\r\n" . wordwrap($text, 75, "\r\n", true),
			'From: altaform@' . (
				!empty($afurl->domain)
				? $afurl->domain
				: 'example.com'
			)
		);
	}




	public static function javascript() {
		global $get;

		// GENERIC ERROR MESSAGE IN JS FILE NOT HOSTED BY US (CROSS-SITE)
		if (in_array($get->message, [
			'Script error',
			'Script error.',
			"TypeError: 'undefined' is not a function"
		]))	return;

		// https://github.com/getsentry/raven-js/issues/756
		if (strpos($get->message, "evaluating 'elt.parentNode'")) return;

		// IGNORE ISSUES WITH CriOS INSERTED JAVASCRIPT FILE
		if (strpos($get->message, '__gCrWeb')) return;

		// IGNORE ISSUES WITH "MULTIFACTOR AUTHENTICATION CLIENT"
		if (strpos($get->message, 'fidoCallback')) return;

		// IGNORE ISSUES WITH "BLINGBAR" TOOLBAR
		if (strpos($get->message, 'BLNGBAR')) return;

		// IGNORE ISSUES WITH "hilitor" - unknown browser add-on
		if (strpos($get->message, 'hilitor')) return;

		// IGNORE ISSUE WITH "atomicFindClose" - unknown
		if (strpos($get->message, 'atomicFindClose')) return;

		// IGNORE SEZNAM SCREENSHOT GENERATOR
		if (strpos($get->message, 'screenshot')) return;

		// IGNORE ZTE "PAGESCROLLMODULE"
		if (strpos($get->message, 'zte')) return;

		// IGNORE JQUERY UI ERRORS - THIS LIBRARY IS RANDOMLY BUGGY
		if (strpos($get->file, 'jquery-ui.min.js')) return;

		// IGNORE ERRORS FROM BOTS. THEY ARE TERRIBAD AT PROCESSING SCRIPTS PROPERLY
		if (preg_match(
			'/bot|crawl|slurp|spider|ucbrowser|wkhtmltopdf|baiduhd|screenshot/i',
			$get->server('HTTP_USER_AGENT'))) {
			return;
		}

		static::log([
			'address'		=> $get->url,
			'details'		=> 'JavaScript: ' . $get->message,
			'error-file'	=> $get->file,
			'error-line'	=> $get->line . ':' . $get->col,
		], false);
	}




	public static function html($item) {
		global $af;

		if (function_exists('\af\cli') && \af\cli()) return $item;

		if (!defined('TBX_SPECIAL_CHARS')) {
			define('TBX_SPECIAL_CHARS', ENT_QUOTES | ENT_HTML5 | ENT_SUBSTITUTE);
		}

		return htmlspecialchars(
			(string) $item,
			TBX_SPECIAL_CHARS,
			'UTF-8',
			true
		);
	}



	public static function json($item, $pretty=true) {
		return !function_exists('json_encode')
			?	print_r($item, true)
			:	@json_encode($item,
					($pretty ? JSON_PRETTY_PRINT : 0) |
					JSON_UNESCAPED_SLASHES |
					JSON_UNESCAPED_UNICODE |
					JSON_PARTIAL_OUTPUT_ON_ERROR
				);
	}



	public static function html_json($item) {
		return static::html(static::json($item));
	}



	public static function nopassword(&$item, $key) {
		if (is_int($key)) return;

		$names = [
			'pass',		'hash',		'salt',		'key',
			'secure',	'token',	'secret',	'crypt',
			'cookie',
		];

		foreach ($names as $name) {
			if (stripos($key, $name) !== false) {
				$item = '********';
			}
		}
	}
}




////////////////////////////////////////////////////////////////////////////////
// HANDLER FOR PHP ERRORS, WARNINGS, NOTICES (BUT NOT EXCEPTIONS)
////////////////////////////////////////////////////////////////////////////////
set_error_handler(function(	$errno,			$errstr,		$errfile=NULL,
							$errline=NULL,	$errcontext=[],	$backtrace=[]) {

	global $afconfig;

	if (!(error_reporting()  &  $errno)) return false;

	$end = true;

	if (function_exists('\af\cli')  &&  !\af\cli()) {
		switch ($errno) {
			case E_WARNING:		case E_USER_WARNING:
			case E_NOTICE:		case E_USER_NOTICE:
				$end = ($afconfig instanceof afConfig)
					? !!$afconfig->debug
					: false;
		}
	}

	afDebug::log([
		'error-code'	=> $errno,
		'details'		=> $errstr,
		'error-file'	=> $errfile,
		'error-line'	=> $errline,
	], $end, $backtrace);

	return $end;
});




////////////////////////////////////////////////////////////////////////////////
// HANDLER FOR PHP EXCEPTIONS (BOTH SYSTEM AND USER GENERATED)
////////////////////////////////////////////////////////////////////////////////
set_exception_handler(function($e) {
	global $afconfig;

	if (!error_reporting()) return;

	$info = [
		'error-code'		=> get_class($e) . ':' . $e->getCode(),
		'details'			=> $e->getMessage(),
		'error-file'		=> $e->getFile(),
		'error-line'		=> $e->getLine(),
	];

	if ($e instanceof pudlConnectionException) {
		$afconfig->debug = false;
		$afconfig->error['email'] = false;
		if (!\af\cli()) \af\error(503, $e->getMessage());
	}

	if (($e instanceof pudlException)  &&  ($e->pudl instanceof pudl)) {
		$info += [
			'ip-database'	=> $e->pudl->server(),
			'db-query'		=> $e->pudl->query(),
		];
	}

	afDebug::log($info, true, $e->getTrace());
});




////////////////////////////////////////////////////////////////////////////////
// CATCH PHP PARSING ERRORS
////////////////////////////////////////////////////////////////////////////////
register_shutdown_function(function() {
	if (!error_reporting()) return;

	$e = error_get_last();
	if (empty($e)) return;

	if ($e['type'] !== E_ERROR  &&  $e['type'] !== E_PARSE) return;

	afDebug::log([
		'error-code'	=> $e['type'],
		'details'		=> $e['message'],
		'error-file'	=> $e['file'],
		'error-line'	=> $e['line'],
	]);
});




////////////////////////////////////////////////////////////////////////////////
// LOG PUDL DATABASE QUERY ERRORS
////////////////////////////////////////////////////////////////////////////////
function afPudlLog($callback, $db, $result=NULL) {
	global $af;

	$path = $af instanceof altaform ? $af->path() : '';

	@file_put_contents(
		$path . '_log/' . @date('Y-m-d') . '-query',
		$db->query() . "\n",
		FILE_APPEND
	);
}




////////////////////////////////////////////////////////////////////////////////
// OUR OWN CUSTOM DATA DUMP FUNCTION
////////////////////////////////////////////////////////////////////////////////
function af_dump($var, $end=true) {
	global $af;

	if ($var instanceof pudlObject) $var = $var->raw();

	if (function_exists('\af\cli') && \af\cli()) {
		if (isset($af)) $af->contentType('txt');
		var_export($var);
		echo "\n";

	} else {
		echo '<pre>';
		var_export($var);
		echo '</pre>';
	}

	if ($end) exit(1);
}
