<?php

class afgeo {

	static $_geoloc = [];


	static function geolocate($location) {
		global $db;

		if (empty($location)) return false;

		if (!isset(self::$_geoloc[$location])) {
			self::$_geoloc[$location] = $db->rowId(
				'pudl_geolocation',
				'location',
				$db->safe($location)
			);
		}

		return self::$_geoloc[$location];
	}



	static function geoip($ipaddress=false) {
		if ($ipaddress === false) $ipaddress = self::ipaddress();
		if ($ipaddress === false) return false;

		$ctx = stream_context_create(['http'=>['timeout'=>1]]);

		$json = @file_get_contents("http://geo.cospix.net/$ipaddress", 0, $ctx);
		if (empty($json)) return false;

		$data = @json_decode($json, true);
		if (empty($data)) return false;

		return $data;
	}



	static function ipaddress() {
		if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
			return $_SERVER['HTTP_X_FORWARDED_FOR'];
		} else if (!empty($_SERVER['REMOTE_ADDR'])) {
			return $_SERVER['REMOTE_ADDR'];
		}
		return false;
	}	



	static function centerMap() {
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
		$geoip = self::geoip();
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
