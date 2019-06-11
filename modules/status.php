<?php




////////////////////////////////////////////////////////////////////////////////
// LIST OF ALL HTTP STATUS CODES
////////////////////////////////////////////////////////////////////////////////
namespace af\status;

$codes = [
	// 1xx - MOSTLY UNUSED
	100 => 'Continue',								//	RFC7231 Section 6.2.1
	101 => 'Switching Protocols',					//	RFC7231 Section 6.2.2
	102 => 'Processing',							//	RFC2518
	103 => 'Early Hints',							//	RFC8297

	// 2xx - GOOD STATUS
	200 => 'OK',									//	RFC7231 Section 6.3.1
	201 => 'Created',								//	RFC7231 Section 6.3.2
	202 => 'Accepted',								//	RFC7231 Section 6.3.3
	203 => 'Non-Authoritative Information',			//	RFC7231 Section 6.3.4
	204 => 'No Content',							//	RFC7231 Section 6.3.5
	205 => 'Reset Content',							//	RFC7231 Section 6.3.6
	206 => 'Partial Content',						//	RFC7233 Section 4.1
	207 => 'Multi-Status',							//	RFC4918
	208 => 'Already Reported',						//	RFC5842
	218 => 'This Is Fine',							//	Apache
	226 => 'IM Used',								//	RFC3229

	// 3xx - REDIRECTS
	300 => 'Multiple Choices',						//	RFC7231 Section 6.4.1
	301 => 'Moved Permanently',						//	RFC7231 Section 6.4.2
	302 => 'Found',									//	RFC7231 Section 6.4.3
	303 => 'See Other',								//	RFC7231 Section 6.4.4
	304 => 'Not Modified',							//	RFC7232 Section 4.1
	305 => 'Use Proxy',								//	RFC7231 Section 6.4.5
	306 => 'Switch Proxy',							//	RFC7231 Section 6.4.6
	307 => 'Temporary Redirect',					//	RFC7231 Section 6.4.7
	308 => 'Permanent Redirect',					//	RFC7538

	// 4xx - CLIENT REQUEST ERRORS
	400 => 'Bad Request',							//	RFC7231 Section 6.5.1
	401 => 'Unauthorized',							//	RFC7235 Section 3.1
	402 => 'Payment Required',						//	RFC7231 Section 6.5.2
	403 => 'Forbidden',								//	RFC7231 Section 6.5.3
	404 => 'Not Found',								//	RFC7231 Section 6.5.4
	405 => 'Method Not Allowed',					//	RFC7231 Section 6.5.5
	406 => 'Not Acceptable',						//	RFC7231 Section 6.5.6
	407 => 'Proxy Authentication Required',			//	RFC7235 Section 3.2
	408 => 'Request Timeout',						//	RFC7231 Section 6.5.7
	409 => 'Conflict',								//	RFC7231 Section 6.5.8
	410 => 'Gone',									//	RFC7231 Section 6.5.9
	411 => 'Length Required',						//	RFC7231 Section 6.5.10
	412 => 'Precondition Failed',					//	RFC7232 Section 4.2
	413 => 'Payload Too Large',						//	RFC7231 Section 6.5.11
	414 => 'URI Too Long',							//	RFC7231 Section 6.5.12
	415 => 'Unsupported Media Type',				//	RFC7231 Section 6.5.13
	416 => 'Range Not Satisfiable',					//	RFC7233 Section 4.4
	417 => 'Expectation Failed',					//	RFC7231 Section 6.5.14
	418 => 'I\'m a teapot',							//	RFC2324 Section 2.3.2
	419 => 'Authentication Timeout',
	420 => 'Method Failure',
	421 => 'Misdirected Request',					//	RFC7540 Section 9.1.2
	422 => 'Unprocessable Entity',					//	RFC4918
	423 => 'Locked',								//	RFC4918
	424 => 'Failed Dependency',						//	RFC4918
	425 => 'Too Early',								//	RFC8470
	426 => 'Upgrade Required',						//	RFC7231 Section 6.5.15
	428 => 'Precondition Required',					//	RFC6585
	429 => 'Too Many Requests',						//	RFC6585
	431 => 'Request Header Fields Too Large',		//	RFC6585
	440 => 'Login Time-out',
	444 => 'No Response',
	449 => 'Retry With',
	450 => 'Blocked by Windows Parental Controls',
	451 => 'Unavailable For Legal Reasons',			//	RFC7725
	494 => 'Request Header Too Large',
	495 => 'SSL Certificate Error',
	496 => 'SSL Certificate Required',
	497 => 'HTTP Request Sent to HTTPS Port',
	499 => 'Client Closed Request',

	// 5xx - SERVER SIDE ERRORS
	500 => 'Internal Server Error',					//	RFC7231, Section 6.6.1
	501 => 'Not Implemented',						//	RFC7231, Section 6.6.2
	502 => 'Bad Gateway',							//	RFC7231, Section 6.6.3
	503 => 'Service Unavailable',					//	RFC7231, Section 6.6.4
	504 => 'Gateway Timeout',						//	RFC7231, Section 6.6.5
	505 => 'HTTP Version Not Supported',			//	RFC7231, Section 6.6.6
	506 => 'Variant Also Negotiates',				//	RFC2295
	507 => 'Insufficient Storage',					//	RFC4918
	508 => 'Loop Detected',							//	RFC5842
	509 => 'Bandwidth Limit Exceeded',
	510 => 'Not Extended',							//	RFC2774
	511 => 'Network Authentication Required',		//	RFC6585
	520 => 'Unknown Error',
	521 => 'Web Server Is Down',
	522 => 'Connection Timed Out',
	523 => 'Origin Is Unreachable',
	524 => 'A Timeout Occurred',
	525 => 'SSL Handshake Failed',
	526 => 'Invalid SSL Certificate',
	527 => 'Railgun Error',
	530 => 'Origin DNS Error',
	598 => 'Network Read Timeout Error',
	599 => 'Unknown Server Error',
];




////////////////////////////////////////////////////////////////////////////////
// DISPLAY THE HTTP ERROR STATUS PAGE FOR THE GIVEN CODE
////////////////////////////////////////////////////////////////////////////////
namespace af;

function error($code, $text=false, $log=false, $details=false) {
	global $http_status_codes, $afurl, $router, $afconfig, $get;

	$code = (int) $code;
	if (empty($http_status_codes[$code])) $code = 599;

	if (is_array($details)) {
		$details	= isset($details['details'])
					? $details['details']
					: 'Unknown Error';
	}

	if ($code === 404  &&  !empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '_DOES_NOT_EXIST_';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			return $afurl->redirect([], 302);
		}

		$text = debug::html($afurl->all) . '<br/>' . $text;

		if (!empty($afconfig->debug)) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
			$text .= '<br/><pre>' . print_r($router,true) . '</pre>';
		}
	}

	return debug::render(
		$code . ' ' . $http_status_codes[$code], [
			'<div id="af-fatal"><h1>ERROR: '.$code.'</h1>',
			'<h2>' . $http_status_codes[$code] . '</h2>',
			'<h3>' . debug::html($details) . '</h3>',
			($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
		],
		$log,
		'error'.$code.'.tpl'
	);
}




////////////////////////////////////////////////////////////////////////////////
// AFFIRM AND SHOW THE GIVEN STATUS CODE ON FAILURE
////////////////////////////////////////////////////////////////////////////////
namespace af;

function affirm($code, $item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->affirm($code, $text, $log);
	return (empty($item) && $item!=='') ? \af\error($code, $text, $log) : $item;
}




////////////////////////////////////////////////////////////////////////////////
// AFFIRM THAT WE HAVE READ, WRITE, OR GRANT PERMISSIONS
////////////////////////////////////////////////////////////////////////////////
namespace af\affirm;

function read($item, $text=false, $log=false, $code=401) {
	return ($item !== true && !in_array($item, ['read', 'write', 'grant']))
			? \af\error($code, $text, $log)
			: $item;
}




////////////////////////////////////////////////////////////////////////////////
// AFFIRM THAT WE HAVE WRITE OR GRANT PERMISSIONS
////////////////////////////////////////////////////////////////////////////////
namespace af\affirm;

function write($item, $text=false, $log=false, $code=401) {
	return ($item !== true && !in_array($item, ['write', 'grant']))
			? \af\error($code, $text, $log)
			: $item;
}




////////////////////////////////////////////////////////////////////////////////
// AFFIRM THAT WE HAVE "GRANT" PERMISSIONS (BASICALLY "ADMIN")
////////////////////////////////////////////////////////////////////////////////
namespace af\affirm;

function grant($item, $text=false, $log=false, $code=401) {
	return ($item !== true && !in_array($item, ['grant']))
			? \af\error($code, $text, $log)
			: $item;
}
