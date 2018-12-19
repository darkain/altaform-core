<?php


class afSystem {




	////////////////////////////////////////////////////////////////////////////
	// GET THE MAIN OS PATH
	////////////////////////////////////////////////////////////////////////////
	public static function path() {
		switch (true) {
			case self::bsd():		return '/usr/local/';
			case self::windows():	return 'c:/';
		}
		return '/';
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE FULL OPERATING SYSTEM STRING
	// http://php.net/manual/en/function.php-uname.php
	////////////////////////////////////////////////////////////////////////////
	public static function name() {
		return php_uname('a');
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE OPERATING SYSTEM NAME
	// http://php.net/manual/en/function.php-uname.php
	////////////////////////////////////////////////////////////////////////////
	public static function os() {
		return php_uname('s');
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE OPERATING SYSTEM RELEASE
	// http://php.net/manual/en/function.php-uname.php
	////////////////////////////////////////////////////////////////////////////
	public static function release() {
		return php_uname('r');
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE OPERATING SYSTEM VERSION
	// http://php.net/manual/en/function.php-uname.php
	////////////////////////////////////////////////////////////////////////////
	public static function version() {
		return php_uname('v');
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE OPERATING SYSTEM ARCHITECTURE (X86, X86_64, ARM, ARM64)
	// http://php.net/manual/en/function.php-uname.php
	////////////////////////////////////////////////////////////////////////////
	public static function arch() {
		return php_uname('m');
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE OS USED TO BUILD THIS PHP BINARY
	// http://php.net/manual/en/reserved.constants.php
	////////////////////////////////////////////////////////////////////////////
	public static function builder() {
		return PHP_OS;
	}




	////////////////////////////////////////////////////////////////////////////
	// BOOLEAN CHECK IF WE'RE ON WINDOWS
	////////////////////////////////////////////////////////////////////////////
	public static function windows() {
		return stripos('WINDOWS', self::os()) !== false;
	}




	////////////////////////////////////////////////////////////////////////////
	// BOOLEAN CHECK IF WE'RE ON (GNU/)LINUX
	////////////////////////////////////////////////////////////////////////////
	public static function linux() {
		return stripos('LINUX', self::os()) !== false;
	}




	////////////////////////////////////////////////////////////////////////////
	// BOOLEAN CHECK IF WE'RE ON FREEBSD
	////////////////////////////////////////////////////////////////////////////
	public static function freebsd() {
		return stripos('FREEBSD', self::os()) !== false;
	}




	////////////////////////////////////////////////////////////////////////////
	// BOOLEAN CHECK IF WE'RE ON BSD (ANY FLAVOR)
	////////////////////////////////////////////////////////////////////////////
	public static function bsd() {
		return stripos('BSD', self::os()) !== false;
	}




	////////////////////////////////////////////////////////////////////////////
	// BOOLEAN CHECK IF WE'RE ON MAC-OS
	////////////////////////////////////////////////////////////////////////////
	public static function mac() {
		return stripos('DARWIN', self::os()) !== false;
	}




	////////////////////////////////////////////////////////////////////////////
	// BOOLEAN CHECK IF WE'RE ON SUN-OS / SOLARIS / SMARTOS
	////////////////////////////////////////////////////////////////////////////
	public static function sun() {
		return stripos('SUNOS', self::os()) !== false;
	}


}