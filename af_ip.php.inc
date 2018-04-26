<?php


class af_ip {


	////////////////////////////////////////////////////////////////////////////
	// CHECK IF A STRING IS A VALID IPv4 OR IPv6 ADDRESS
	// REQUIRES FILTER PHP MODULE: http://php.net/manual/en/filter.filters.php
	////////////////////////////////////////////////////////////////////////////
	public static function valid($address) {
		return filter_var($address, FILTER_VALIDATE_IP);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF A STRING IS A VALID IPv4 ADDRESS
	// REQUIRES FILTER PHP MODULE: http://php.net/manual/en/filter.filters.php
	////////////////////////////////////////////////////////////////////////////
	public static function valid4($address) {
		return filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4);
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF A STRING IS A VALID IPv6 ADDRESS
	// REQUIRES FILTER PHP MODULE: http://php.net/manual/en/filter.filters.php
	////////////////////////////////////////////////////////////////////////////
	public static function valid6($address) {
		return filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE REMOTE CLIENT'S IP ADDRESS, OR '127.0.0.1' FOR COMMAND LINE
	////////////////////////////////////////////////////////////////////////////
	public static function address() {
		global $get;
		if (altaform::cli()) return '127.0.0.1';
		$address = $get->server('HTTP_X_FORWARDED_FOR');
		if (empty($address)) $address = $get->server('REMOTE_ADDR');
		return empty($address) ? false : $address;
	}




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF A GIVEN IP ADDRESS IS WITHIN A GIVEN CIDR LIST (IPv4 ONLY)
	////////////////////////////////////////////////////////////////////////////
	public static function inCidr($address, $cidr) {
		if (!tbx_array($cidr)) $cidr = [$cidr];
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




	////////////////////////////////////////////////////////////////////////////
	// CHECK IF CLIENT'S IP ADDRESS IS WITHIN A GIVEN CIDR LIST (IPv4 ONLY)
	////////////////////////////////////////////////////////////////////////////
	public static function addressInCidr($cidr) {
		return static::inCidr(static::address(), $cidr);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE IP ADDRESS OF THIS PHP INSTANCE (IPv4 ONLY)
	////////////////////////////////////////////////////////////////////////////
	public static function local() {
		$address	= false;
		$socket		= socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
		socket_connect(		$socket, '8.8.8.8', 53);
		socket_getsockname(	$socket, $address);
		return $address;
	}


}
