<?php


define('BYTE_FORMAT_K',		0x0001);
define('BYTE_FORMAT_KB',	0x0002);
define('BYTE_FORMAT_KIB',	0x0003);


class afstring {
	static function url($string) {
		return strtolower(urlencode($string));
	}


	static function int($string) {
		if (is_int($string)) return $string;
		if (!is_string($string)) return 0;
		$string = trim($string);
		if (!ctype_digit($string)) return 0;
		return (int) $string;
	}


	static function striphtml($string, $length=false) {
		$string = preg_replace('#<[^>]+>#', ' ', $string);
		$string = str_replace('&nbsp;', ' ', $string);
		$string = preg_replace('/\s\s+/', ' ', $string);
		$string = trim($string);
		if (is_numeric($length) && $length >= 1) $string = self::truncateWord($string, $length);
		return $string;
	}


	static function truncateword($string, $length) {
		$length = (int) $length;
		if (strlen($string) <= $length) return $string;
		return preg_replace('/\s+?(\S+)?$/', '', substr($string, 0, $length+1));
	}


	static function linkify($string) {
		$string = preg_replace(
			'@(?<![.*>])\b(?:(?:https?)://|(?<![./*>])((www|m)\.)|([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}))[-A-Z0-9+&#/%=~_|$?!:,.]*[A-Z0-9+&#/%=~_|$]@i',
			'<a href="\0" target="_blank">\0</a>',
			htmlspecialchars($string, ENT_NOQUOTES)
		);

		return str_replace(
			["\r","\n",  '<a href="www.',		'<a href="m.'],
			['','<br/>', '<a href="http://www.','<a href="http://m.'],
			$string
		);
	}


	static function daterange($start, $end) {
		$start	= (int) $start;
		$end	= (int) $end;
		$text	= date('F jS', $start);

		if ((int)($start / 86400)  ==  (int)($end / 86400)) {
		} else if (date('n', $start) === date('n', $end)) {
			$text .= '-' . date('jS', $end);
		} else if (date('Y', $start) !== date('Y', $end)) {
			$text .= ', '  . date('Y', $start);
			$text .= ' - ' . date('F jS', $end);
		} else {
			$text .= ' - ' . date('F jS', $end);
		}

		return $text . ', ' . date('Y', $end);
	}


	static function implode($array) {
		if (empty($array)  ||  !is_array($array)) return '';
		if (count($array) === 1) return reset($array);
		if (count($array) === 2) return reset($array) . ' and ' . end($array);
		$last = array_pop($array);
		return implode(', ', $array) . ', and ' . $last;
	}


	static function embed($path, $mimetype=false) {
		$text = '';
		if (!empty($mimetype)) $text .= "data:$mimetype;base64,";
		return $text . base64_encode(file_get_contents($path));
	}


	static function regex($string) {
		$string = str_replace('\\',	'\\\\\\\\',	$string);
		$string = str_replace('/',	'\\\\/',	$string);
		$string = str_replace('[',	'\\\\[',	$string);
		$string = str_replace(']',	'\\\\]',	$string);
		$string = str_replace('|',	'\\\\|',	$string);
		$string = str_replace('(',	'\\\\(',	$string);
		$string = str_replace(')',	'\\\\)',	$string);
		$string = str_replace('{',	'\\\\{',	$string);
		$string = str_replace('}',	'\\\\}',	$string);
		$string = str_replace('$',	'\\\\$',	$string);
		$string = str_replace('.',	'\\\\.',	$string);
		$string = str_replace('^',	'\\\\^',	$string);
		$string = str_replace('+',	'\\\\+',	$string);
		$string = str_replace('-',	'\\\\-',	$string);
		$string = str_replace('*',	'\\\\*',	$string);
		$string = str_replace('?',	'\\\\?',	$string);
		$string = str_replace('"',	'\\\\"',	$string);
		$string = str_replace("'",	"\\\\'",	$string);
		$string = rtrim($string, '\\');
		return $string;
	}


	static function encrypt($data, $secret=false) {
		global $site;
		if (empty($secret)) $secret = $site['af_secret'];
		$key	= pack('H*', $secret);
		$size	= mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_CBC);
		$iv		= mcrypt_create_iv($size, MCRYPT_RAND);
		$cipher	= mcrypt_encrypt(MCRYPT_RIJNDAEL_128, $key, $data, MCRYPT_MODE_CBC, $iv);
		$encode = base64_encode($iv . $cipher);
		$encode = str_replace('+', '-', $encode);
		$encode = str_replace('/', '_', $encode);
		$encode = str_replace('=', '~', $encode);
		return $encode;
	}


	static function decrypt($data, $secret=false) {
		global $site;
		if (empty($data)) return false;
		if (empty($secret)) $secret = $site['af_secret'];
		$key	= pack('H*', $secret);
		$data	= str_replace('-', '+', $data);
		$data	= str_replace('_', '/', $data);
		$data	= str_replace('~', '=', $data);
		$data	= @base64_decode($data, true);
		if (empty($data)) return false;
		$size	= mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_CBC);
		$iv		= substr($data, 0, $size);
		$text	= substr($data, $size);
		$value	= mcrypt_decrypt(MCRYPT_RIJNDAEL_128, $key, $text, MCRYPT_MODE_CBC, $iv);
		return rtrim ($value, "\0\4");
	}



	function maxDigits($value, $digits) {
		return round($value, max(0, $digits-strlen((string)round($value))));
	}


	function fromBytes($value, $format=false, $sep=' ') {
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
				return self::maxDigits( ($value / pow($size, $i)), 4 ) . $sep . $units[$i];
			}
		}
		return false;
	}


	function toBytes($string) {
		$value = (float)trim($string);
		$rest  = trim(substr($string, strlen((string)$value)));
		switch (strtoupper($rest)) {  //TODO: strip out all non-alpha characters!
			case 'B': return round($value);
			case 'K': case 'KIB': return round($value * pow(1024, 1));
			case 'M': case 'MIB': return round($value * pow(1024, 2));
			case 'G': case 'GIB': return round($value * pow(1024, 3));
			case 'T': case 'TIB': return round($value * pow(1024, 4));
			case 'P': case 'PIB': return round($value * pow(1024, 5));
			case 'E': case 'EIB': return round($value * pow(1024, 6));
			case 'Z': case 'ZIB': return round($value * pow(1024, 7));
			case 'Y': case 'YIB': return round($value * pow(1024, 8));
			case 'KB': return round($value * pow(1000, 1));
			case 'MB': return round($value * pow(1000, 2));
			case 'GB': return round($value * pow(1000, 3));
			case 'TB': return round($value * pow(1000, 4));
			case 'PB': return round($value * pow(1000, 5));
			case 'EB': return round($value * pow(1000, 6));
			case 'ZB': return round($value * pow(1000, 7));
			case 'YB': return round($value * pow(1000, 8));
		}
		return 0;
	}



	static function language($input) {
		foreach (self::$languages as $key => $val) {
			preg_match_all($val, $input, $language[$key]);
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


	static $languages = [
		'eng' => '/[a-z]/i',
		'jpn' => '/[\x{3040}-\x{30ff}]/u',
		'kor' => '/[\x{3130}-\x{318f}\x{ac00}-\x{d7af}]/u',
		'chi' => '/[\x{2e80}-\x{2eff}\x{3000}-\x{303f}\x{3200}-\x{9fff}]/u',
		'rus' => '/[\x{0400}-\x{052f}]/u',
		'heb' => '/[\x{0590}-\x{05ff}]/u',
	];
}




function prettyExif(array &$exif, $html=true) {
	if (!empty($exif['FocalLength'])) {
		$pos = strpos($exif['FocalLength'], '/');
		if (!empty($pos)) {
			$num1 = (int)substr($exif['FocalLength'],0,$pos);
			$num2 = (int)substr($exif['FocalLength'],$pos+1);
			if ($num1 > 0  &&  $num2 > 0) {
				$exif['FocalLength'] = $num1 / $num2;
			} else {
				$exif['FocalLength'] = 0;
			}
		}
	}

	foreach ($exif as &$item) {
		if (is_array($item)) {
			prettyExif($item);
		} else if (is_string($item)) {
			$item = str_replace('f/', 'ƒ/', $item);
			$item = htmlspecialchars($item);

			if ($html) {
				$href = '|(https?://([\d\w\.-]+\.[\w\.]{2,6})[^\s\]\[\<\>]*/?)|i';
				$url  = '<a href="$1" target="_blank">$2</a>';
				$item = preg_replace($href, $url, $item);
			}
		}
	}
}
