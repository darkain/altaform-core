<?php

class afGeo {

	static $_geoloc = [];



	////////////////////////////////////////////////////////////////////////////
	// USE GOOGLE MAPS API TO FIGURE OUT A LOCATION'S LOCAL TIME ZONE
	////////////////////////////////////////////////////////////////////////////
	public static function timezone($latitude, $longitude=false) {
		global $af;

		if (empty($af)  ||  empty($af->config)) return false;

		if ($longitude === false) {
			$geocode = is_object($latitude) ? $latitude : static::geocode($latitude);

			if (empty($geocode->results[0]->geometry->location)) return false;

			$latitude	= $geocode->results[0]->geometry->location->lat;
			$longitude	= $geocode->results[0]->geometry->location->lng;
		}


		return @json_decode(
			@file_get_contents(
				'https://maps.googleapis.com/maps/api/timezone/json' .
				'?location=' . (float)$latitude . ',' . (float)$longitude .
				'&timestamp=' . $af->time() .
				'&key=' . $af->config->google['timezone']
			)
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// USE GOOGLE MAPS API TO TRANSLATE A LOCATION NAME INTO LAT/LON
	////////////////////////////////////////////////////////////////////////////
	public static function geocode($location) {
		global $af;

		if (is_null($af)  ||  is_null($af->config)) return NULL;
		if (trim($location, " \t\n\r\0\x0B,") === '') return NULL;


		return @json_decode(
			@file_get_contents(
				'https://maps.googleapis.com/maps/api/geocode/json' .
				'?address=' . rawurlencode($location) .
				'&key=' . $af->config->google['geocoding']
			)
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET A CACHED GEOLOCATION FROM DATABASE
	////////////////////////////////////////////////////////////////////////////
	public static function geolocate($location) {
		global $db;

		if (is_null($db)  ||  empty($location)) return false;

		if (!array_key_exists($location, self::$_geoloc)) {
			self::$_geoloc[$location] = $db->rowId(
				'pudl_geolocation',
				'location',
				$location
			);
		}

		return self::$_geoloc[$location];
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE LOCATION OF A USER BASED ON THEIR IP ADDRESS
	// IF ADDRESS IS FALSE, USE THE CURRENT CLIENT'S IP ADDRESS
	////////////////////////////////////////////////////////////////////////////
	public static function geoip($ipaddress=false) {
		global $af;

		if (is_null($af)  ||  is_null($af->config)) return false;
		if (empty($af->config->geo)) return false;

		if (empty($ipaddress)) $ipaddress = afIp::address();
		if (empty($ipaddress)) return false;

		$ctx = stream_context_create(['http'=>['timeout'=>1]]);

		$json = @file_get_contents($af->config->geo.$ipaddress, false, $ctx);
		if (empty($json)) return false;

		$data = @json_decode($json, true);

		return (is_array($data)  &&  !empty($data))
			? $data
			: false;
	}




	////////////////////////////////////////////////////////////////////////////
	// RETURNS AN ARRAY WITH THE LAT, LON, AND ZOOM LEVEL OF A MAP
	////////////////////////////////////////////////////////////////////////////
	public static function centerMap() {
		global $user, $get;

		//Specific map center requested by URL
		if (($get->float('lat') !== 0  ||  $get->float('lon') !== 0)
			&&  $get->float('zoom') > 0) {
			return [
				'lat'	=> $get->float('lat'),
				'lon'	=> $get->float('lon'),
				'zoom'	=> $get->float('zoom')
			];
		}

		//User's profile default location
		if (!empty($user['user_lat'])  &&  !empty($user['user_lat'])) {
			return [
				'lat'	=> $user['user_lat'],
				'lon'	=> $user['user_lon'],
				'zoom'	=> 7
			];
		}

		//Geolocate User
		$geoip = static::geoip();
		if (!empty($geoip['latitude']) && !empty($geoip['longitude'])) {
			return [
				'lat'	=> $geoip['latitude'],
				'lon'	=> $geoip['longitude'],
				'zoom'	=> 7
			];
		}

		//Default: North America
		return [
			'lat'	=> 41,
			'lon'	=> -100,
			'zoom'	=> 4
		];
	}




	////////////////////////////////////////////////////////////////////////////
	// SANITIZE A LOCATION NAME TO MATCH COSPIX FORMATTING.
	// MAY BE USEFUL TO OTHERS, TOO. IT HELPS WITH GOOGLE MAPS GEOLOCATION API
	////////////////////////////////////////////////////////////////////////////
	function clean_location_name($location) {
		global $db;

		$location = strtolower($location);
		$location = ucwords($location);
		$location = str_replace(',', ', ', $location);
		$location = str_replace('. ', ' ', $location);

		$location = afString::stripwhitespace($location, ' ');
		$location = afString::reducewhitespace($location);

		$location = preg_replace('/\\busa?\\b/i', '', $location);
		$location = preg_replace('/\\bunited states( of america)?\\b/i', '', $location);

		$location = trim($location);

		while (substr($location, -1) === ',') {
			$location = substr($location, 0, strlen($location)-1);
		}

		$states = $db->rows('pudl_state');

		foreach ($states as $state) {
			if (preg_match("/, $state[state_name]\\b/i", $location)) {
				$location = preg_replace("/, $state[state_name]\\b/i", ", $state[state_code]", $location, 1);
				break;
			} else if (preg_match("/, $state[state_code]\\b/i", $location)) {
				$location = preg_replace("/, $state[state_code]\\b/i", ", $state[state_code]", $location, 1);
				break;
			} else if (preg_match("/.\\b$state[state_name]\\b/i", $location)) {
				$location = preg_replace("/.\\b$state[state_name]\\b/i", ", $state[state_code]", $location, 1);
				break;
			} else if (preg_match("/.\\b$state[state_code]\\b/i", $location)) {
				$location = preg_replace("/.\\b$state[state_code]\\b/i", ", $state[state_code]", $location, 1);
				break;
			}
		}

		$location = str_replace(' ,', ',', $location);
		while (stripos($location, ',,') !== false) {
			$location = str_replace(',,', ',', $location);
		}

		return trim($location);
	}


}
