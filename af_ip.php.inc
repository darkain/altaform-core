<?php


class af_ip {


	public static function valid($address) {
		return filter_var($address, FILTER_VALIDATE_IP);
	}


	public static function valid4($address) {
		return filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4);
	}


	public static function valid6($address) {
		return filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6);
	}



	public static function address() {
		global $get;
		$address = $get->server('HTTP_X_FORWARDED_FOR');
		if (empty($address)) $address = $get->server('REMOTE_ADDR');
		return empty($address) ? false : $address;
	}



	public static function inCidr($address, $cidr) {
		if (!is_array($cidr)) $cidr = [$cidr];
		if (empty($address)) return false;
		$address		= ip2long(trim($address));
		foreach ($cidr as $item) {
			list($network, $mask) = explode('/', $item, 2);
			$network	= ip2long(trim($network));
			$mask		= ~((1 << (32 - (int)trim($mask))) - 1);
			if (($address & $mask) === ($network & $mask)) return true;
		}
		return false;
	}



	public static function addressInCidr($cidr) {
		return self::inCidr(self::address(), $cidr);
	}


}
