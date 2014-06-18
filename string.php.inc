<?php


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



function string_to_int($string) {
	if (is_int($string)) return $string;
	if (!is_string($string)) return 0;
	$string = trim($string);
	if (!ctype_digit($string)) return 0;
	return (int) $string;
}



function truncateWord($string, $length) {
	$length = (int) $length;
	if (strlen($string) <= $length) return $string;
	return preg_replace('/\s+?(\S+)?$/', '', substr($string, 0, $length+1));
}



function strip_html($string, $length=false) {
	$string = preg_replace('#<[^>]+>#', ' ', $string);
	$string = str_replace('&nbsp;', ' ', $string);
	$string = preg_replace('/\s\s+/', ' ', $string);
	$string = trim($string);
	if (is_numeric($length) && $length >= 1) $string = truncateWord($string, $length);
	return $string;
}




function getPageTitle($url) {
	//We have to LIE to Facebook in order to get the proper page title!
	//TODO: apparently we now need to LOG INTO Facebook just to see galleries!
	ini_set('user_agent', 'Mozilla/5.0 (X11; U; Linux x86_64; en-US) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.43 Safari/537.31');

	$str = file_get_contents($url);
	if (!empty($str)) {
		preg_match("/\<title[^>]*>(.*)\<\/title\>/", $str, $title);
		var_dump($title);
		if (!empty($title[1])) return htmlspecialchars_decode($title[1]);
	}
	return false;
}




function getPageName($url) {
	$parts = parse_url($url);

	$pos1  = strrpos($parts['host'], '.');
	if ($pos1 === false) return false;

	$pos2  = strrpos($parts['host'], '.', $pos1-strlen($parts['host'])-1);
	$domain = ($pos2 === false) ? ($parts['host']) : (substr($parts['host'], $pos2+1));

	switch ($domain) {
		case 'fb.me':	case 'fb.com':	case 'facebook.com':
			return 'facebook';

		case 'blogger.com':	case 'blogspot.com':
			return 'blogger';

		case 'deviantart.com':	case 'fav.me':
			return 'deviantart';

		case 'youtube.com':	case 'youtu.be':
			return 'youtube';

		case 'twitter.com':	case 't.co':
			return 'twitter';

		case 'flickr.com':	case 'flic.kr':
			return 'flickr';

		case 'tumblr.com':	case 'tmblr.co':
			return 'tumblr';

		case 'livejournal.com':	return 'livejournal';
		case 'digg.com':		return 'digg';
		case 'reddit.com':		return 'reddit';
		case 'google.com':		return 'google';
		case 'feedburner.com':	return 'feedburner';
		case 'lastfm.com':		return 'lastfm';
		case 'linkedin.com':	return 'linkedin';
		case 'myspace.com':		return 'myspace';
		case 'steam.com':		return 'steam';
		case 'stumbleupon.com':	return 'stumbleupon';
		case 'technorati.com':	return 'technorati';
		case 'twitpic.com':		return 'twitpic';
		case 'vimeo.com':		return 'vimeo';
	}

	return false;
}
