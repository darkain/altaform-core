<?php


namespace af;



define('AF_SECOND',		  1);
define('AF_MINUTE',		 60 * AF_SECOND);
define('AF_HOUR',		 60 * AF_MINUTE);
define('AF_DAY',		 24 * AF_HOUR);
define('AF_WEEK',		  7 * AF_DAY);
define('AF_MONTH',		 30 * AF_DAY);
define('AF_YEAR',		365 * AF_DAY);
define('AF_DECADE',		 10 * AF_YEAR);
define('AF_CENTURY',	 10 * AF_DECADE);




class time {

	////////////////////////////////////////////////////////////////////////////
	// A LIST OF TIME PERIOD CHUNKS
	////////////////////////////////////////////////////////////////////////////
	public static $chunks = [
		[AF_YEAR,	'year'],
		[AF_MONTH,	'month'],
		[AF_WEEK,	'week'],
		[AF_DAY,	'day'],
		[AF_HOUR,	'hour'],
		[AF_MINUTE,	'minute'],
	];




	////////////////////////////////////////////////////////////////////////////
	// GET A UNIX TIMESTAMP FROM THE GIVEN DATE STRING OR NUMBER
	////////////////////////////////////////////////////////////////////////////
	public static function get($data) {
		if (is_int($data))		return $data;
		if (ctype_digit($data))	return (int) $data;
		return strtotime($data);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET A UNIX TIMESTAMP OFFSET FROM ALTAFORM EXECUTION TIME
	////////////////////////////////////////////////////////////////////////////
	public static function from($offset, $precision=0) {
		global $af;
		if (!$precision) return $af->time() - $offset;
		return (floor($af->time()/$precision)*$precision) - $offset;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE ELAPSED TIME SINCE THE GIVEN UNIX TIMESTAMP
	////////////////////////////////////////////////////////////////////////////
	public static function since($timestamp, $maxoffset=AF_DAY) {
		global $af;
		if ($timestamp == 0) return 'Never';

		if ($af->time() > $timestamp) {
			$since = $af->time() - $timestamp;
			if ($since == 1)		return "1 second";
			if ($since < AF_MINUTE)	return "$since seconds";
		} else {
			$since = $timestamp - $af->time();
			if ($since == 0)		return 'Now';
			if ($since == 1)		return '1 second';
			if ($since < AF_MINUTE)	return "$since seconds";
		}

		if ($maxoffset > 0  &&  $since > $maxoffset) {
			return date('F jS, Y \a\t g:i A', $timestamp);
		}

		// $j saves performing the count function each time around the loop
		for ($i = 0, $j = count(self::$chunks); $i < $j; $i++) {
			$seconds	= self::$chunks[$i][0];
			$name		= self::$chunks[$i][1];
			// finding the biggest chunk (if the chunk fits, break)
			if (($count = floor($since / $seconds)) > 1) break;
		}

		$print = ($count == 1) ? '1 '.$name : "$count {$name}s";

		for ($x = $i+1;		$x < $j;	$x++) {
			// now getting the second item
			$seconds2	= self::$chunks[$x][0];
			$name2		= self::$chunks[$x][1];

			// add second item if it's count greater than 0
			if (($count2 = floor(($since - ($seconds * $count)) / $seconds2)) != 0) {
				$print .= ($count2 == 1) ? ', 1 '.$name2 : ", $count2 {$name2}s";
				break;
			}
		}

		return $print;
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT ISO-8601 DURATION INTO NUMBER OF SECONDS
	////////////////////////////////////////////////////////////////////////////
	public static function duration($duration) {
		preg_match(
			'/^(-|)?P([0-9.]+Y|)?([0-9.]+M|)?([0-9.]+D|)?T?([0-9.]+H|)?([0-9.]+M|)?([0-9.]+S|)?$/',
			str_replace(',', '.', $duration),
			$matches
		);

		if (count($matches) < 8) return 0;

		return	(int)(
				((float)$matches[7])
			+  (((float)$matches[6]) * AF_MINUTE)
			+  (((float)$matches[5]) * AF_HOUR)
			+  (((float)$matches[4]) * AF_DAY)
			+  (((float)$matches[3]) * AF_MONTH)
			+  (((float)$matches[2]) * AF_YEAR));
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE NEAREST TIME ZONE
	////////////////////////////////////////////////////////////////////////////
	public static function nearestZone($lat, $lon) {
		global $af;

		$path  = 'https://maps.googleapis.com/maps/api/timezone/json?';
		$path .= afUrl::query([
			'location'		=> implode(',', [(float)$lat, (float)$lon]),
			'timestamp'		=> $af->time(),
			'sensor'		=> false,
			'key'			=> $af->config->google['server_key'],
		]);

		$data = @file_get_contents($path);
		$json = @json_decode($data, true);

		return !empty($json) ? $json : false;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET A HUMAN READABLE DATE RANGE FROM TWO UNIX TIMESTAMPS
	////////////////////////////////////////////////////////////////////////////
	static function daterange($start, $end, $month='F', $year=', Y') {
		$start	= (int) $start;
		$end	= (int) $end;
		$text	= date($month.' jS', $start);

		if (floor($start / AF_DAY)  ===  floor($end / AF_DAY)) {
			// DO NOTHING

		} else if (date('n', $start) === date('n', $end)) {
			$text .= '-' . date('jS', $end);

		} else if (date('Y', $start) !== date('Y', $end)) {
			$text .= date($year, $start);
			$text .= ' - ' . date($month.' jS', $end);

		} else {
			$text .= ' - ' . date($month.' jS', $end);
		}

		return $text . date($year, $end);
	}

}
