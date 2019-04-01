<?php



define('BYTE_FORMAT_K',		0x0001);
define('BYTE_FORMAT_KB',	0x0002);
define('BYTE_FORMAT_KIB',	0x0003);




class afString {




	////////////////////////////////////////////////////////////////////////////
	// CONVERT A STRING TO AN INTEGER
	////////////////////////////////////////////////////////////////////////////
	public static function int($string) {
		if (is_int($string))		return $string;
		if (!is_string($string))	return 0;
		$string = trim($string);
		if (!ctype_digit($string))	return 0;
		return (int) $string;
	}




	////////////////////////////////////////////////////////////////////////////
	// STRIP CURRENCY SYMBOL FROM STRING, AND RETURN VALUE AS A FLOAT
	////////////////////////////////////////////////////////////////////////////
	public static function currency($value) {
		return (float) preg_replace(
			'/^[\$\s\x{A2}-\x{A5}\x{20A0}-\x{20CF}\x{10192}]+/u',
			'', $value
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT A VALUE TO A STRING
	////////////////////////////////////////////////////////////////////////////
	public static function string($value) {
		if (is_string($value))	return $value;
		if (!is_object($value))	return @(string)$value;

		if (method_exists($value, '__toString')) {
			return $value->__toString();

		} elseif ($value instanceof DateTime) {
			return $value->format('c');
		}

		return @(string)$value;
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE A KEY/VALUE STRING FROM AN ARRAY
	////////////////////////////////////////////////////////////////////////////
	public static function attribute($array) {
		$output = ' ';

		foreach ($array as $key => $value) {
			if (is_int($key)) {
				$output .= altaform::html($value) . ' ';
				continue;
			}

			$output .= altaform::html($key);
			$output .= '="';
			$output .= altaform::html($value);
			$output .= '" ';
		}

		return rtrim($output, ' ');
	}




	////////////////////////////////////////////////////////////////////////////
	// REMOVE CONSECUTIVE WHITE-SPACE CHARACTERS, REPLACING WITH A SINGLE SPACE
	////////////////////////////////////////////////////////////////////////////
	public static function doublespace($string) {
		return preg_replace('/[\0\s][\0\s]+/', ' ', (string)$string);
	}




	////////////////////////////////////////////////////////////////////////////
	// SAME AS DOUBLESPACE, WHILE ALSO TRIMMING THE ENDS
	////////////////////////////////////////////////////////////////////////////
	public static function doubletrim($string) {
		return trim(static::doublespace($string));
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT ASCII SLASH TO UNICODE SLASH
	////////////////////////////////////////////////////////////////////////////
	public static function slash($value) {
		return str_replace('/', '⁄', $value);
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT UNICODE SLASH TO ASCII SLASH
	////////////////////////////////////////////////////////////////////////////
	public static function unslash($value) {
		return str_replace('⁄', '/', $value);
	}




	////////////////////////////////////////////////////////////////////////////
	// REMOVE HTML FROM STRING
	////////////////////////////////////////////////////////////////////////////
	public static function striphtml($string, $length=false) {
		$string = preg_replace('#<[^>]+>#', ' ', $string);
		$string = str_replace('&nbsp;', ' ', $string);
		$string = static::doublespace($string);
		$string = trim($string);
		if (is_numeric($length) && $length >= 1) {
			$string = static::truncateWord($string, $length);
		}
		return $string;
	}




	////////////////////////////////////////////////////////////////////////////
	// REMOTE WHITE-SPACE, INCLUDING LEGACY URL ENCODED '+' SIGN
	////////////////////////////////////////////////////////////////////////////
	public static function stripwhitespace($string, $replace='') {
		return str_replace(
			['+', ' ', "\t", "\r", "\n", "\0", "\x0B"],
			$replace,
			$string
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// ???
	////////////////////////////////////////////////////////////////////////////
	public static function truncateword($string, $length) {
		$length = (int) $length;
		if (strlen($string) <= $length) return $string;
		return preg_replace('/\s+?(\S+)?$/', '', substr($string, 0, $length+1));
	}




	////////////////////////////////////////////////////////////////////////////
	// REPLACE TEXT THAT LOOKS LIKE A URL WITH A FULL ANCHOR TAG
	////////////////////////////////////////////////////////////////////////////
	public static function linkify($string) {
		$string = preg_replace(
			'@(?<![.*>])\b(?:(?:(ht|f)tps?)://|(?<![./*>])((www|m)\.)|([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}))[-A-Z0-9+&#/%=~_|$?!:;,.]*[A-Z0-9+&#/%=~_|$]@i',
			'<a href="\0" target="_blank">\0</a>',
			altaform::html($string, ENT_NOQUOTES | ENT_HTML5 | ENT_SUBSTITUTE)
		);

		return str_replace(["\r\n","\r","\n"], '<br/>', $string);
	}




	////////////////////////////////////////////////////////////////////////////
	// HUMAN READABLE ARRAY IMPLODING
	////////////////////////////////////////////////////////////////////////////
	public static function implode($array, $final='and') {
		if (empty($array)  ||  !tbx_array($array)) return '';
		if (count($array) === 1) return reset($array);
		if (count($array) === 2) return implode(' ', reset($array), $final, end($array));

		$last		= array_pop($array);
		$array[]	= $final . ' ' . $last;
		return implode(', ', $array);
	}




	////////////////////////////////////////////////////////////////////////////
	// EMBED A BINARY FILE DIRECTLY INTO A STRING
	////////////////////////////////////////////////////////////////////////////
	public static function embed($path, $mimetype=false) {
		$text = '';
		if (!empty($mimetype)) $text .= "data:$mimetype;base64,";
		return $text . base64_encode(file_get_contents($path));
	}




	////////////////////////////////////////////////////////////////////////////
	// ???
	////////////////////////////////////////////////////////////////////////////
	public static function regex($string) {
		$string = str_replace('\\',	'\\\\',	$string);
		$string = str_replace('/',	'\\/',	$string);
		$string = str_replace('[',	'\\[',	$string);
		$string = str_replace(']',	'\\]',	$string);
		$string = str_replace('|',	'\\|',	$string);
		$string = str_replace('(',	'\\(',	$string);
		$string = str_replace(')',	'\\)',	$string);
		$string = str_replace('{',	'\\{',	$string);
		$string = str_replace('}',	'\\}',	$string);
		$string = str_replace('$',	'\\$',	$string);
		$string = str_replace('.',	'\\.',	$string);
		$string = str_replace('^',	'\\^',	$string);
		$string = str_replace('+',	'\\+',	$string);
		$string = str_replace('-',	'\\-',	$string);
		$string = str_replace('*',	'\\*',	$string);
		$string = str_replace('?',	'\\?',	$string);
		$string = str_replace('"',	'\\"',	$string);
		$string = str_replace("'",	"\\'",	$string);
		$string = rtrim($string, '\\');
		return $string;
	}




	////////////////////////////////////////////////////////////////////////////
	// PASS IN A FLOAT, GET A RATIONAL: PASS IN (66.66667) AND GET "66-2/3"
	// https://stackoverflow.com/questions/14330713
	////////////////////////////////////////////////////////////////////////////
	static function rational($n, $tolerance=1.e-4) {
		$n = (float) $n;
		if (empty($n)) return '';

		$h1=1;		$h2=0;
		$k1=0;		$k2=1;
		$b = 1/$n;

		do {
			$b = 1/$b;
			$a = floor($b);
			$x = $h1;		$h1 = $a*$h1+$h2;		$h2 = $x;
			$x = $k1;		$k1 = $a*$k1+$k2;		$k2 = $x;
			$b = $b-$a;
		} while (abs($n-$h1/$k1) > $n*$tolerance);

		$r1 = (int)($h1 / $k1);
		$r2 = (int)($h1 % $k1);

		if ($r1  &&  $r2) return (string)($r1 . '-' . $r2 . '/' . $k1);
		if ($r2) return (string)($r2 . '/' . $k1);
		if ($r1) return (string)($r1);
		return '';
	}




	////////////////////////////////////////////////////////////////////////////
	// PASS IN A RATIONAL, GET A FLOAT
	// EXAMPLE: "66-2/3" BECOMES (66.6667)
	////////////////////////////////////////////////////////////////////////////
	static function unrational($string, $round=4) {
		$value		= 0.0;
		$rational	= [];

		$parts = preg_split('/[\s+-]/', $string, 2);
		if (count($parts) === 2) {
			$value		= (float)(int) trim($parts[0]);
			$rational	= explode('/', $parts[1], 2);
		} else {
			$rational	= explode('/', $parts[0], 2);
			if (count($rational) === 1) {
				$value	= (float)(int) trim($parts[0]);
			}
		}

		if (count($rational) === 2) {
			$rational[0] = (float)(int) trim($rational[0]);
			$rational[1] = (float)(int) trim($rational[1]);
			if ($rational[0] !== 0  &&  $rational[1] !== 0) {
				$value += round($rational[0] / $rational[1], $round);
			}
		}

		return $value;
	}




	////////////////////////////////////////////////////////////////////////////
	// ???
	////////////////////////////////////////////////////////////////////////////
	public static function maxDigits($value, $digits) {
		return round($value, max(0, $digits-strlen((string)round($value))));
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT FROM INTEGER TO FILE SIZE STRING
	////////////////////////////////////////////////////////////////////////////
	public static function fromBytes($value, $format=false, $sep=' ') {
		if ($format === false) $format = BYTE_FORMAT_KIB;

		switch ($format) {
			case BYTE_FORMAT_K:
				$units	= ['B', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'];
				$size	= 1024;
			break;

			case BYTE_FORMAT_KB:
				$units	= ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
				$size	= 1000;
			break;

			case BYTE_FORMAT_KIB:
				$units	= ['B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB'];
				$size	= 1024;
			break;

			default: return false;
		}

		for ($i=0; $i<count($units); $i++) {
			if ( ($value >= pow($size, $i) )  &&  ($value <= pow($size, $i+1)) ) {
				return static::maxDigits( ($value / pow($size, $i)), 4 ) . $sep . $units[$i];
			}
		}

		return false;
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT FROM FILE SIZE STRING TO INTEGER
	////////////////////////////////////////////////////////////////////////////
	public static function toBytes($string, $precision=0, $mode=PHP_ROUND_HALF_UP) {
		$value	= (float)trim($string);
		$rest	= trim(substr($string, strlen((string)$value)));
		$rest	= preg_replace('/[^A-Z]/', '', strtoupper($rest));
		switch (strtoupper($rest)) {
			case '':
			case 'B':
				return (int) round($value, $precision, $mode);

			case 'K':
			case 'KIB':
				return (int) round($value * AF_KILOBYTE, $precision, $mode);

			case 'M':
			case 'MIB':
				return (int) round($value * AF_MEGABYTE, $precision, $mode);

			case 'G':
			case 'GIB':
				return (int) round($value * AF_GIGABYTE, $precision, $mode);

			case 'T':
			case 'TIB':
				return (int) round($value * AF_TERABYTE, $precision, $mode);

			case 'P':
			case 'PIB':
				return (int) round($value * AF_PETABYTE, $precision, $mode);

			case 'E':
			case 'EIB':
				return (int) round($value * AF_EXABYTE, $precision, $mode);

			case 'Z':
			case 'ZIB':
				return (int) round($value * AF_ZETTABYTE, $precision, $mode);

			case 'Y':
			case 'YIB':
				return (int) round($value * AF_YOTTABYTE, $precision, $mode);

			case 'KB':
				return (int) round($value * pow(1000, 1), $precision, $mode);

			case 'MB':
				return (int) round($value * pow(1000, 2), $precision, $mode);

			case 'GB':
				return (int) round($value * pow(1000, 3), $precision, $mode);

			case 'TB':
				return (int) round($value * pow(1000, 4), $precision, $mode);

			case 'PB':
				return (int) round($value * pow(1000, 5), $precision, $mode);

			case 'EB':
				return (int) round($value * pow(1000, 6), $precision, $mode);

			case 'ZB':
				return (int) round($value * pow(1000, 7), $precision, $mode);

			case 'YB':
				return (int) round($value * pow(1000, 8), $precision, $mode);
		}
		return 0;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF THE GIVEN STRING IS ASCII ENCODED
	////////////////////////////////////////////////////////////////////////////
	public static function ascii($string) {
		return (bool) (extension_loaded('mbstring')
			? @mb_check_encoding($string, 'ASCII')
			: !@preg_match('/[^\x09\x0A\x0D\x20-\x7f]/', $string));
	}





	////////////////////////////////////////////////////////////////////////////
	// CHECK IF THE GIVEN STRING IS UTF-8 ENCODED
	////////////////////////////////////////////////////////////////////////////
	public static function utf8($string) {
		return (bool) (extension_loaded('mbstring')
			? @mb_check_encoding($string, 'UTF-8')
			: @preg_match('//u', $string));
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT THE GIVEN STRING FROM UTF-8 TO ASCII ENCODING
	////////////////////////////////////////////////////////////////////////////
	public static function toAscii($string, $from='UTF-8') {
		return extension_loaded('mbstring')
			? @mb_convert_encoding($string, $from, 'ASCII')
			: @iconv($from, 'ASCII//TRANSLIT', $string);
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT THE GIVEN STRING FROM ASCII TO UTF-8 ENCODING
	////////////////////////////////////////////////////////////////////////////
	public static function toUtf8($string, $from='ASCII') {
		return extension_loaded('mbstring')
			? @mb_convert_encoding($string, $from, 'UTF-8')
			: @iconv($from, 'UTF-8//TRANSLIT', $string);
	}




	////////////////////////////////////////////////////////////////////////////
	// ???
	////////////////////////////////////////////////////////////////////////////
	public static function removeBreaks($string) {
		return preg_replace('/[\pZ\pC]/u', ' ', $string);
	}




	////////////////////////////////////////////////////////////////////////////
	// ATTEMPT TO DETECT NATIVE SPOKEN LANGUAGE
	////////////////////////////////////////////////////////////////////////////
	public static function language($input) {
		$language = [];

		foreach (self::$languages as $key => $val) {
			preg_match_all($val, $input, $tmp);
			$language[$key] = $tmp;
		}

		// Reduce our array hell down to the counts we actually care about
		foreach ($language as &$val) {
			$val = !empty($val[0]) ? count($val[0]) : 0;
		} unset($val);

		// Add the Chinese character count to the korean and japanese if they're >0
		if ($language['jpn']) $language['jpn'] += $language['chi'];
		if ($language['kor']) $language['kor'] += $language['chi'];

		// Return the key of the largest language in our list
		return max($language) ? array_flip($language)[max($language)] : 'eng';
	}




	////////////////////////////////////////////////////////////////////////////
	// LISTING OF NATIVE LANGAUGES
	////////////////////////////////////////////////////////////////////////////
	public static $languages = [
		'eng' => '/[a-z]/i',
		'jpn' => '/[\x{3040}-\x{30ff}]/u',
		'kor' => '/[\x{3130}-\x{318f}\x{ac00}-\x{d7af}]/u',
		'chi' => '/[\x{2e80}-\x{2eff}\x{3000}-\x{303f}\x{3200}-\x{9fff}]/u',
		'rus' => '/[\x{0400}-\x{052f}]/u',
		'heb' => '/[\x{0590}-\x{05ff}]/u',
	];
}
