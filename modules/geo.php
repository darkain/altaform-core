<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING GEOLOCATION TASKS
////////////////////////////////////////////////////////////////////////////////
class geo {


	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR, REQUIRES INSTANCE OF AFCONFIG
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\afConfig $config) {
		$this->config = $config;
	}




	////////////////////////////////////////////////////////////////////////////
	// USE GOOGLE MAPS API TO FIGURE OUT A LOCATION'S LOCAL TIME ZONE
	////////////////////////////////////////////////////////////////////////////
	public function timezone($latitude, $longitude=NULL) {
		if ($longitude === NULL  ||  $longitude === false) {
			$geocode = is_object($latitude) ? $latitude : $this->geocode($latitude);

			if (empty($geocode->results[0]->geometry->location)) return NULL;

			$latitude	= $geocode->results[0]->geometry->location->lat;
			$longitude	= $geocode->results[0]->geometry->location->lng;
		}


		return @json_decode(
			@file_get_contents(
				'https://maps.googleapis.com/maps/api/timezone/json' .
				'?location=' . (float)$latitude . ',' . (float)$longitude .
				'&timestamp=' . time() .
				'&key=' . $this->config->google['timezone']
			)
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// USE GOOGLE MAPS API TO TRANSLATE A LOCATION NAME INTO LAT/LON
	////////////////////////////////////////////////////////////////////////////
	public function geocode($location) {
		if (trim($location, " \t\n\r\0\x0B,") === '') return NULL;

		return @json_decode(
			@file_get_contents(
				'https://maps.googleapis.com/maps/api/geocode/json' .
				'?address=' . rawurlencode($location) .
				'&key=' . $this->config->google['geocoding']
			)
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET A CACHED GEOLOCATION FROM DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function geolocate(\pudl $pudl, $location) {
		if (empty($pudl)  ||  empty($location)) return NULL;

		static $cache = [];

		if (!array_key_exists($location, $cache)) {
			$cache[$location] = $pudl->cache(AF_HOUR)->rowId(
				'geolocation',
				'location',
				$location
			);
		}

		return $cache[$location];
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE LOCATION OF A USER BASED ON THEIR IP ADDRESS
	// IF ADDRESS IS FALSE, USE THE CURRENT CLIENT'S IP ADDRESS
	////////////////////////////////////////////////////////////////////////////
	public function geoip($ipaddress=false) {
		if (empty($this->config->geo)) return NULL;

		if (empty($ipaddress)) $ipaddress = ip::address();
		if (empty($ipaddress)) return NULL;

		$ctx = stream_context_create(['http'=>['timeout'=>1]]);

		$json = @file_get_contents($this->config->geo.$ipaddress, false, $ctx);
		if (empty($json)) return NULL;

		$data = @json_decode($json, true);

		return (is_array($data)  &&  !empty($data))
			? $data
			: NULL;
	}




	////////////////////////////////////////////////////////////////////////////
	// RETURNS AN ARRAY WITH THE LAT, LON, AND ZOOM LEVEL OF A MAP
	////////////////////////////////////////////////////////////////////////////
	public function center($user=NULL, $get=NULL) {

		// Specific map center requested by URL
		if (is_object($get)) {
			if (($get->float('lat') !== 0  ||  $get->float('lon') !== 0)
				&&  $get->float('zoom') > 0) {
				return [
					'lat'	=> $get->float('lat'),
					'lon'	=> $get->float('lon'),
					'zoom'	=> $get->float('zoom')
				];
			}
		}

		// User's profile default location
		if (is_object($user)) {
			if (isset($user->user_lat)  &&  isset($user->user_lat)) {
				return [
					'lat'	=> $user->user_lat,
					'lon'	=> $user->user_lon,
					'zoom'	=> 7
				];
			}
		}

		// Geolocate User
		$geoip = $this->geoip();
		if (is_array($geoip)) {
			return [
				'lat'	=> $geoip['latitude'],
				'lon'	=> $geoip['longitude'],
				'zoom'	=> 7
			];
		}


		// Use default, since we cannot detect!
		return $this->usa();
	}




	////////////////////////////////////////////////////////////////////////////
	// DEFAULT VALUE WHEN NO OTHER GEOLOCATION INFORMATION IS AVAILABLE
	// http://www.kansastravel.org/geographicalcenter.htm
	////////////////////////////////////////////////////////////////////////////
	public function usa() {
		return [
			'lat'	=> 39.82834,
			'lon'	=> -98.57948,
			'zoom'	=> 4
		];
	}




	////////////////////////////////////////////////////////////////////////////
	// SANITIZE A LOCATION NAME TO MATCH ALTAFORM FORMATTING.
	// MAY BE USEFUL TO OTHERS, TOO. IT HELPS WITH GOOGLE MAPS GEOLOCATION API
	////////////////////////////////////////////////////////////////////////////
	function clean(\pudl $pudl, $location) {
		$location = strtolower($location);
		$location = ucwords($location);
		$location = str_replace(',', ', ', $location);
		$location = str_replace('. ', ' ', $location);

		$location = \afString::stripwhitespace($location, ' ');
		$location = \afString::doublespace($location);

		$location = preg_replace('/\\busa?\\b/i', '', $location);
		$location = preg_replace('/\\bunited states( of america)?\\b/i', '', $location);

		$location = trim($location);

		while (substr($location, -1) === ',') {
			$location = substr($location, 0, strlen($location)-1);
		}

		$states = $pudl->rows('state');

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




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private $config = NULL;

}
