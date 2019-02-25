<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR HANDLING COMMON IP ADDRESS TASKS
////////////////////////////////////////////////////////////////////////////////
class ip {


	////////////////////////////////////////////////////////////////////////////
	// GET THE CURRENT SERVER'S HOSTNAME
	// http://php.net/manual/en/function.gethostname.php
	////////////////////////////////////////////////////////////////////////////
	public static function host() {
		return gethostname();
	}




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
		if (function_exists('\af\cli') && cli()) {
			return '127.0.0.1';
		}

		if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
			return $_SERVER['HTTP_X_FORWARDED_FOR'];
		}

		return !empty($_SERVER['REMOTE_ADDR'])
			? $_SERVER['REMOTE_ADDR']
			: NULL;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE HTTPD SERVER ADDRESS (MAY BE IPv4 OR IPv6)
	// THIS MAY BE DIFFERENT DUE TO PHP-FPM AND LOAD BALANCING BETWEEN NODES
	////////////////////////////////////////////////////////////////////////////
	public static function server() {
		if (function_exists('\af\cli') && cli()) {
			return '127.0.0.1';
		}

		return !empty($_SERVER['SERVER_ADDR'])
			? $_SERVER['SERVER_ADDR']
			: NULL;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE IP ADDRESS OF THIS PHP INSTANCE (IPv4 ONLY)
	////////////////////////////////////////////////////////////////////////////
	public static function local() {
		$address	= false;
		if (function_exists('socket_create')) {
			$socket	= @socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
			if ($socket !== false) {
				@socket_connect(	$socket, '8.8.8.8', 53);
				@socket_getsockname($socket, $address);
				@socket_close(		$socket);
			}
		}
		return $address;
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



}
