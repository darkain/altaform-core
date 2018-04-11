<?php

class afgeo {

	static $_geoloc = [];



	public static function timezone($latitude, $longitude=false) {
		global $af;

		if ($longitude === false) {
			$geocode = is_object($latitude) ? $latitude : static::geocode($latitude);
			if (empty($geocode->results[0]->geometry->location)) return false;
			$latitude	= $geocode->results[0]->geometry->location->lat;
			$longitude	= $geocode->results[0]->geometry->location->lng;
		}

		$url  = 'https://maps.googleapis.com/maps/api/timezone/json';
		$url .= '?location=' . (float)$latitude . ',' . (float)$longitude;
		$url .= '&timestamp=' . $af->time();
		$url .= '&key=' . $af->config->google['timezone'];

		return @json_decode(
			@file_get_contents($url)
		);
	}



	public static function geocode($location) {
		global $af;

		if (trim($location, " \t\n\r\0\x0B,") === '') return NULL;

		$url  = 'https://maps.googleapis.com/maps/api/geocode/json';
		$url .= '?address=' . rawurlencode($location);
		$url .= '&key=' . $af->config->google['geocoding'];

		return @json_decode(
			@file_get_contents($url)
		);
	}



	public static function geolocate($location) {
		global $db;

		if (empty($location)) return false;

		if (!isset(self::$_geoloc[$location])) {
			self::$_geoloc[$location] = $db->rowId(
				'pudl_geolocation',
				'location',
				$location
			);
		}

		return self::$_geoloc[$location];
	}



	public static function geoip($ipaddress=false) {
		if ($ipaddress === false) $ipaddress = static::ipaddress();
		if ($ipaddress === false) return false;

		$ctx = stream_context_create(['http'=>['timeout'=>1]]);

		$json = @file_get_contents("http://geo.cospix.net/$ipaddress", 0, $ctx);
		if (empty($json)) return false;

		$data = @json_decode($json, true);
		if (empty($data)) return false;

		return $data;
	}



	public static function ipaddress() {
		return af_ip::address();
	}



	public static function centerMap() {
		global $user, $get;

		//Specific map center requested by URL
		if (($get->float('lat') !== 0  ||  $get->float('lon') !== 0)  &&  $get->float('zoom') > 0) {
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

}
