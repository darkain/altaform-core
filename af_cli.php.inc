<?php


class af_cli {

	public static function line() { return "\r\n"; }

	public static function code($code) { return "\033[" . $code . 'm'; }

	public static function reset() { return static::code(0); }

	public static function bold(		$enabled=true)	{ return static::code($enabled ? 1 : 21); }
	public static function dim(			$enabled=true)	{ return static::code($enabled ? 2 : 22); }
	public static function underlined(	$enabled=true)	{ return static::code($enabled ? 4 : 24); }
	public static function blink(		$enabled=true)	{ return static::code($enabled ? 5 : 25); }
	public static function reverse(		$enabled=true)	{ return static::code($enabled ? 7 : 27); }
	public static function hidden(		$enabled=true)	{ return static::code($enabled ? 8 : 28); }

	public static function fg_black()					{ return static::code(30); }
	public static function fg_red(		$light=false)	{ return static::code($light ? 91 : 31); }
	public static function fg_green(	$light=false)	{ return static::code($light ? 92 : 32); }
	public static function fg_yellow(	$light=false)	{ return static::code($light ? 93 : 33); }
	public static function fg_blue(		$light=false)	{ return static::code($light ? 94 : 34); }
	public static function fg_magenta(	$light=false)	{ return static::code($light ? 95 : 35); }
	public static function fg_cyan(		$light=false)	{ return static::code($light ? 96 : 36); }
	public static function fg_gray(		$light=false)	{ return static::code($light ? 37 : 90); }
	public static function fg_white(	$light=false)	{ return static::code($light ? 39 : 97); }

	public static function bg_black()					{ return static::code(40); }
	public static function bg_red(		$light=false)	{ return static::code($light ? 101 : 41); }
	public static function bg_green(	$light=false)	{ return static::code($light ? 102 : 42); }
	public static function bg_yellow(	$light=false)	{ return static::code($light ? 103 : 43); }
	public static function bg_blue(		$light=false)	{ return static::code($light ? 104 : 44); }
	public static function bg_magenta(	$light=false)	{ return static::code($light ? 105 : 45); }
	public static function bg_cyan(		$light=false)	{ return static::code($light ? 106 : 46); }
	public static function bg_gray(		$light=false)	{ return static::code($light ? 47 : 100); }
	public static function bg_white(	$light=false)	{ return static::code($light ? 49 : 107); }

}
