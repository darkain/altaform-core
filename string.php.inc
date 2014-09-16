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
			'@(?<![.*">])\b(?:(?:https?)://)[-A-Z0-9+&#/%=~_|$?!:,.]*[A-Z0-9+&#/%=~_|$]@i',
			'<a href="\0" target="_blank">\0</a>',
			htmlspecialchars($string)
		);

		return str_replace(["\r","\n"], ['','<br/>'], $string);
	}


	static function implode($array) {
		if (empty($array)  ||  !is_array($array)) return '';
		if (count($array) === 1) return reset($array);
		if (count($array) === 2) return reset($array) . ' and ' . end($array);
		$last = array_pop($array);
		return implode(', ', $array) . ', and ' . $last;
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
