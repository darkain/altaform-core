<?php


class afException extends Exception {}



class afError {


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
			if (function_exists('afCli') && afCli()) {
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
		global $af, $afurl, $afrouter, $db, $get, $user;

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
			'ip-php'		=> !class_exists('afIp')	? '' :	afIp::local(),
			'ip-client'		=> !class_exists('afIp')	? '' :	afIp::address(),
			'ip-httpd'		=> !class_exists('afIp')	? '' :	afIp::server(),
		], $data);

		if (!empty($afrouter->redirected)) {
			$return['redirected'] = static::json($afrouter->redirected, false);
		}

		ksort($return, SORT_NATURAL);
		return $return;
	}




	public static function log($data, $die=true, $backtrace=[]) {
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

		$error = (ob_get_level()  &&  $die) ? static::html(ob_get_clean()) : '';

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


		if (!$die) {
			error_reporting($olderr);
			return $arr;
		}


		if (empty($afconfig->debug)) return error500('', true, $arr);


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

		error500($html, true, $arr);
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
		if ($get->message === 'Script error') return;
		if ($get->message === 'Script error.') return;

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

		if (function_exists('afCli') && afCli()) return $item;

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

		switch ($key) {
			case 'pass':
			case 'password':
			case 'hash':
			case 'salt':
			case 'key':
			case 'secure':
				$item = '********';
		}
	}
}




function error400($text=false, $log=false, $details=false) {
	afError::render('400 Bad Request', [
		'<div id="af-fatal"><h1>ERROR: 400</h1>',
		'<h2>BAD REQUEST</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error400.tpl');
}




function error401($text=false, $log=false, $details=false) {
	afError::render('401 Unauthorized', [
		'<div id="af-fatal"><h1>ERROR: 401</h1>',
		'<h2>AUTHORIZATION REQUIRED</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error401.tpl');
}




function error402($text=false, $log=false, $details=false) {
	afError::render('402 Payment Required', [
		'<div id="af-fatal"><h1>ERROR: 402</h1>',
		'<h2>PAYMENT REQUIRED</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error402.tpl');
}




function error403($text=false, $log=false, $details=false) {
	afError::render('403 Forbidden', [
		'<div id="af-fatal"><h1>ERROR: 403</h1>',
		'<h2>FORBIDDEN</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error403.tpl');
}




function error404($text=false, $log=false, $details=false) {
	global $afurl, $get, $afconfig;

	if (!empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '_DOES_NOT_EXIST_';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			$afurl->redirect([], 302);
		}

		$text = afError::html($afurl->all) . '<br/>' . $text;

		if (!empty($afconfig->debug)) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
		}
	}

	afError::render('404 File Not Found', [
		'<div id="af-fatal"><h1>ERROR: 404</h1>',
		'<h2>FILE NOT FOUND</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error404.tpl');
}




function error405($text=false, $log=true, $details=false) {
	afError::render('405 Method Not Allowed', [
		'<div id="af-fatal"><h1>ERROR: 405</h1>',
		'<h2>METHOD NOT ALLOWED</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error405.tpl');
}




function error422($text=false, $log=true, $details=false) {
	afError::render('422 Unprocessable Entity', [
		'<div id="af-fatal"><h1>ERROR: 422</h1>',
		'<h2>UNPROCESSABLE ENTITY</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error422.tpl');
}




function error500($text=false, $log=true, $details=false) {
	if (is_array($details)) $details = $details['details'];

	afError::render('500 Internal Server Error', [
		'<div id="af-fatal"><h1>ERROR: 500</h1>',
		'<h2>INTERNAL SERVER ERROR</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error500.tpl');
}




function error503($text=false, $log=true, $details=false) {
	afError::render('503 Service Unavailable', [
		'<div id="af-fatal"><h1>ERROR: 503</h1>',
		'<h2>SERVICE UNAVAILABLE</h2>',
		'<h3>' . afError::html($details) . '</h3>',
		($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
	], $log, 'error503.tpl');
}




function assert400($item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assert400($text);
	return (empty($item) && $item!=='') ? error400($text, $log) : $item;
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


function assert503($item, $text=false, $log=true) {
	if ($item instanceof pudlOrm) return $item->assert503($text);
	return (empty($item) && $item!=='') ? error503($text, $log) : $item;
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




////////////////////////////////////////////////////////////////////////////////
// HANDLER FOR PHP ERRORS, WARNINGS, NOTICES (BUT NOT EXCEPTIONS)
////////////////////////////////////////////////////////////////////////////////
set_error_handler(function(	$errno,			$errstr,		$errfile=NULL,
							$errline=NULL,	$errcontext=[],	$backtrace=[]) {

	global $afconfig;

	if (!(error_reporting()  &  $errno)) return false;

	$die = true;

	if (function_exists('afCli')  &&  !afCli()) {
		switch ($errno) {
			case E_WARNING:		case E_USER_WARNING:
			case E_NOTICE:		case E_USER_NOTICE:
				$die = ($afconfig instanceof afConfig)
					? !!$afconfig->debug
					: false;
		}
	}

	afError::log([
		'error-code'	=> $errno,
		'details'		=> $errstr,
		'error-file'	=> $errfile,
		'error-line'	=> $errline,
	], $die, $backtrace);

	return $die;
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

	if ($e instanceof pudlException) {
		if ($e->getCode() === PUDL_X_CONNECTION) {
			$afconfig->debug = false;
			$afconfig->error['email'] = false;
			if (!afCli()) error503($e->getMessage());
		}

		if ($e->db instanceof pudl) $info += [
			'ip-database'	=> $e->db->server(),
			'db-query'		=> $e->db->query(),
		];
	}

	afError::log($info, true, $e->getTrace());
});




////////////////////////////////////////////////////////////////////////////////
// CATCH PHP PARSING ERRORS
////////////////////////////////////////////////////////////////////////////////
register_shutdown_function(function() {
	if (!error_reporting()) return;

	$e = error_get_last();
	if (empty($e)) return;

	if ($e['type'] !== E_ERROR  &&  $e['type'] !== E_PARSE) return;

	afError::log([
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
function af_dump($var, $die=true) {
	global $af;

	if ($var instanceof pudlObject) $var = $var->raw();

	if (function_exists('afCli') && afCli()) {
		if (isset($af)) $af->contentType('txt');
		var_export($var);
		echo "\n";

	} else {
		echo '<pre>';
		var_export($var);
		echo '</pre>';
	}

	if ($die) exit(1);
}
