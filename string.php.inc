<?php


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
