<?php


class af_ip {


	public static function address() {
		global $get;
		$address = $get->server('HTTP_X_FORWARDED_FOR');
		if (empty($address)) $address = $get->server('REMOTE_ADDR');
		return empty($address) ? false : $address;
	}



	public static function inCidr($address, $cidr) {
		list($network, $mask) = split('/', $cidr);
		$mask = (int) $mask;
		$mask = ~((1 << (32 - $mask)) - 1);
		return (ip2long($address) & $mask) === (ip2long($network) & $mask);
	}


}
