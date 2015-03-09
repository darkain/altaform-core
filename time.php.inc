<?php

class aftime {

	// array of time period chunks
	public static $chunks = [
		[31536000, 'year'],
		[ 2592000, 'month'],
		[  604800, 'week'],
		[   86400, 'day'],
		[    3600, 'hour'],
		[      60, 'minute'],
	];


	public static function since($timestamp, $maxoffset=86400) {
		global $af;
		if ($timestamp == 0) return "Never";

		if ($af->time() > $timestamp) {
			$tense = ' ago';
			$since = $af->time() - $timestamp;
			if ($since == 1) return "1 second$tense";
			if ($since < 60) return "$since seconds$tense";
		} else {
			$tense = '';
			$since = $timestamp - $af->time();
			if ($since == 0) return 'Now';
			if ($since == 1) return '1 second';
			if ($since < 60) return "$since seconds";
		}

		if ($maxoffset > 0  &&  $since > $maxoffset) return date('F jS, Y \a\t g:i A', $timestamp);

		// $j saves performing the count function each time around the loop
		for ($i = 0, $j = count(self::$chunks); $i < $j; $i++) {
			$seconds = self::$chunks[$i][0];
			$name = self::$chunks[$i][1];
			// finding the biggest chunk (if the chunk fits, break)
			if (($count = floor($since / $seconds)) > 1) break;
		}

		$print = ($count == 1) ? '1 '.$name : "$count {$name}s";

		for ($x = $i+1;	$x < $j;	$x++) {
			// now getting the second item
			$seconds2 = self::$chunks[$x][0];
			$name2 = self::$chunks[$x][1];

			// add second item if it's count greater than 0
			if (($count2 = floor(($since - ($seconds * $count)) / $seconds2)) != 0) {
				$print .= ($count2 == 1) ? ', 1 '.$name2 : ", $count2 {$name2}s";
				break;
			}
		}

		return $print . $tense;
	}



	//TODO: WE NEED A BETTER IMPLEMENTATION. RENO NV DOESN'T WORK
	public static function nearestZone($lat, $lon, $country=false) {
		if ($country) {
			$list = DateTimeZone::listIdentifiers(DateTimeZone::PER_COUNTRY, $country);
		} else {
			$list = DateTimeZone::listIdentifiers();
		}

		$zone = false;
		$tz_dist = 0;

		if(!isset($list[0])) return $zone;

		if (count($list) === 1) return $list[0];

		foreach($list as &$item) {
			$timezone	= new DateTimeZone($item);
			$location	= $timezone->getLocation();
			$tz_lat		= $location['latitude'];
			$tz_long	= $location['longitude'];

			$theta		= $lon - $tz_long;
			$dist		= (sin(deg2rad($lat)) * sin(deg2rad($tz_lat)))
						+ (cos(deg2rad($lat)) * cos(deg2rad($tz_lat)) * cos(deg2rad($theta)));
			$dist		= acos($dist);
			$dist		= abs(rad2deg($dist));

			if (!$zone || $tz_dist > $dist) {
				$zone		= $timezone;
				$tz_dist	= $dist;
			}
		}

		return $zone;
	}

}
