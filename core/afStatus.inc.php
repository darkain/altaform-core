<?php


////////////////////////////////////////////////////////////////////////////////
// LIST OF ALL HTTP STATUS CODES
////////////////////////////////////////////////////////////////////////////////
$http_status_codes = [
	// 1xx - MOSTLY UNUSED
	100 => 'Continue',
	101 => 'Switching Protocols',
	102 => 'Processing',
	103 => 'Early Hints',

	// 2xx - GOOD STATUS
	200 => 'OK',
	201 => 'Created',
	202 => 'Accepted',
	203 => 'Non-Authoritative Information',
	204 => 'No Content',
	205 => 'Reset Content',
	206 => 'Partial Content',
	207 => 'Multi-Status',
	208 => 'Already Reported',
	218 => 'This Is Fine',
	226 => 'IM Used',

	// 3xx - REDIRECTS
	300 => 'Multiple Choices',
	301 => 'Moved Permanently',
	302 => 'Found',
	303 => 'See Other',
	304 => 'Not Modified',
	305 => 'Use Proxy',
	306 => 'Switch Proxy',
	307 => 'Temporary Redirect',
	308 => 'Permanent Redirect',

	// 4xx - CLIENT REQUEST ERRORS
	400 => 'Bad Request',
	401 => 'Unauthorized',
	402 => 'Payment Required',
	403 => 'Forbidden',
	404 => 'Not Found',
	405 => 'Method Not Allowed',
	406 => 'Not Acceptable',
	407 => 'Proxy Authentication Required',
	408 => 'Request Timeout',
	409 => 'Conflict',
	410 => 'Gone',
	411 => 'Length Required',
	412 => 'Precondition Failed',
	413 => 'Payload Too Large',
	414 => 'URI Too Long',
	415 => 'Unsupported Media Type',
	416 => 'Range Not Satisfiable',
	417 => 'Expectation Failed',
	418 => 'I\'m a teapot',
	419 => 'Page Expired',
	420 => 'Method Failure',
	421 => 'Misdirected Request',
	422 => 'Unprocessable Entity',
	423 => 'Locked',
	424 => 'Failed Dependency',
	425 => 'Unordered Collection',
	426 => 'Upgrade Required',
	428 => 'Precondition Required',
	429 => 'Too Many Requests',
	431 => 'Request Header Fields Too Large',
	440 => 'Login Time-out',
	444 => 'No Response',
	449 => 'Retry With',
	450 => 'Blocked by Windows Parental Controls',
	451 => 'Unavailable For Legal Reasons',
	494 => 'Request Header Too Large',
	495 => 'SSL Certificate Error',
	496 => 'SSL Certificate Required',
	497 => 'HTTP Request Sent to HTTPS Port',
	499 => 'Client Closed Request',

	// 5xx - SERVER SIDE ERRORS
	500 => 'Internal Server Error',
	501 => 'Not Implemented',
	502 => 'Bad Gateway',
	503 => 'Service Unavailable',
	504 => 'Gateway Timeout',
	505 => 'HTTP Version Not Supported',
	506 => 'Variant Also Negotiates',
	507 => 'Insufficient Storage',
	508 => 'Loop Detected',
	509 => 'Bandwidth Limit Exceeded',
	510 => 'Not Extended',
	511 => 'Network Authentication Required',
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
function httpError($code, $text=false, $log=false, $details=false) {
	global $http_status_codes, $afurl, $afconfig, $get;

	$code = (int) $code;
	if (empty($http_status_codes[$code])) $code = 599;

	//TODO: SPECIAL HANDLER FOR ERROR 404

	if ($code === 404  &&  !empty($afurl)) {
		if (empty($afurl->all)) $afurl->all = '_DOES_NOT_EXIST_';
		if ($get->server('HTTP_REFERER') === $afurl->all) {
			return $afurl->redirect([], 302);
		}

		$text = afDebug::html($afurl->all) . '<br/>' . $text;

		if (!empty($afconfig->debug)) {
			$text .= '<br/><pre>' . print_r($afurl,true) . '</pre>';
		}
	}

	return afDebug::render(
		$code . ' ' . $http_status_codes[$code], [
			'<div id="af-fatal"><h1>ERROR: '.$code.'</h1>',
			'<h2>' . $http_status_codes[$code] . '</h2>',
			'<h3>' . afDebug::html($details) . '</h3>',
			($text !== false ? '<i>' . $text . '</i>' : '') . '</div>',
		],
		$log,
		'error'.$code.'.tpl'
	);
}




////////////////////////////////////////////////////////////////////////////////
// ASSERT AND SHOW THE GIVEN STATUS CODE ON FAILURE
////////////////////////////////////////////////////////////////////////////////
function assertStatus($code, $item, $text=false, $log=false) {
	if ($item instanceof pudlOrm) return $item->assertStatus($code, $text, $log);
	return (empty($item) && $item!=='') ? httpError($code, $text, $log) : $item;
}




////////////////////////////////////////////////////////////////////////////////
// ASSERT THAT WE HAVE READ, WRITE, OR GRANT PERMISSIONS
////////////////////////////////////////////////////////////////////////////////
function assertRead($item, $text=false, $log=false) {
	return ($item !== true && !in_array($item, ['read', 'write', 'grant']))
		? error401($text, $log) : $item;
}




////////////////////////////////////////////////////////////////////////////////
// ASSERT THAT WE HAVE WRITE OR GRANT PERMISSIONS
////////////////////////////////////////////////////////////////////////////////
function assertWrite($item, $text=false, $log=false) {
	return ($item !== true && !in_array($item, ['write', 'grant']))
		? error401($text, $log) : $item;
}




////////////////////////////////////////////////////////////////////////////////
// ASSERT THAT WE HAVE "GRANT" PERMISSIONS (BASICALLY "ADMIN")
////////////////////////////////////////////////////////////////////////////////
function assertGrant($item, $text=false, $log=false) {
	return ($item !== true && !in_array($item, ['grant']))
		? error401($text, $log) : $item;
}
