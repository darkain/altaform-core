<?php
require_once('../error.php.inc');

$list = get_loaded_extensions();

$need = [
	'ctype',		// cytpe_int
	'curl',			// read/write HTTP(s) GET/POST streams
	'exif',			// photo metadata
	'filter',		// IPv4/IPv6 filter
	'hash',			// ??? - encryption
	'iconv',		// UTF-8 conversion
	'imagick',		// photo processing
	'json',			// javascript object notation
//	'mail',			// error reporting and user account registration emails
	'mysqli',		// database
	'openssl',		// fopen('https//example.com') HTTPS
	'redis',		// database caching
	'session',		// ???
	'sockets',		// ???
	'tokenizer',	// ???
	'xml',			// ???
	'zlib',			// gzip compress static files
];

foreach ($need as $item) {
	assert500(
		in_array($item, $list),
		'Required PHP module not found: ' . $item
	);
}


if (!empty($af)) {
	$af->ok();
} else {
	echo "GOOD!\n";
}
