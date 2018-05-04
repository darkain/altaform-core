<?php


////////////////////////////////////////////////////////////////////////////////
// SHOTCUT FUNCTION TO ACCESS CURRENT CLI STATUS
////////////////////////////////////////////////////////////////////////////////
function afCli() { return afCli::cli(); }




class afCli {

	////////////////////////////////////////////////////////////////////////////
	// WITHOUT PARAMETER, GET THE CURRENT CLI STATUS
	// WITH PARAMETER, GET THE FORMATTED CONSOLE STRING OF THE GIVEN CODE
	////////////////////////////////////////////////////////////////////////////
	public function invoke($code=NULL) {
		return ($code === NULL) ? static::cli() : static::code($code);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE CURRENT CLI STATUS
	// TRUE:	WE'RE ON THE COMMAND LINE
	// FALSE:	WE'RE NOT, PROBABLY HTTP(S)
	////////////////////////////////////////////////////////////////////////////
	public static function cli() {
		if (php_sapi_name() === 'cli')					return true;
		if (defined('STDIN'))							return true;
		if (!empty($_SERVER['REQUEST_METHOD']))			return false;
		return empty($_SERVER['argc']);
	}

	public static function line()		{ return PHP_EOL; }
	public static function code($code)	{ return "\033[" . $code . 'm'; }
	public static function reset()		{ return static::code(0); }

	public static function bold(		$enabled=true)	{ return static::code($enabled ? 1 : 21); }
	public static function dim(			$enabled=true)	{ return static::code($enabled ? 2 : 22); }
	public static function underlined(	$enabled=true)	{ return static::code($enabled ? 4 : 24); }
	public static function blink(		$enabled=true)	{ return static::code($enabled ? 5 : 25); }
	public static function reverse(		$enabled=true)	{ return static::code($enabled ? 7 : 27); }
	public static function hidden(		$enabled=true)	{ return static::code($enabled ? 8 : 28); }

	public static function fgBlack()					{ return static::code(30); }
	public static function fgRed(		$light=false)	{ return static::code($light ? 91 : 31); }
	public static function fgGreen(		$light=false)	{ return static::code($light ? 92 : 32); }
	public static function fgYellow(	$light=false)	{ return static::code($light ? 93 : 33); }
	public static function fgBlue(		$light=false)	{ return static::code($light ? 94 : 34); }
	public static function fgMagenta(	$light=false)	{ return static::code($light ? 95 : 35); }
	public static function fgCyan(		$light=false)	{ return static::code($light ? 96 : 36); }
	public static function fgGray(		$light=false)	{ return static::code($light ? 37 : 90); }
	public static function fgWhite(		$light=false)	{ return static::code($light ? 39 : 97); }

	public static function bgBlack()					{ return static::code(40); }
	public static function bgRed(		$light=false)	{ return static::code($light ? 101 : 41); }
	public static function bgGreen(		$light=false)	{ return static::code($light ? 102 : 42); }
	public static function bgYellow(	$light=false)	{ return static::code($light ? 103 : 43); }
	public static function bgBlue(		$light=false)	{ return static::code($light ? 104 : 44); }
	public static function bgMagenta(	$light=false)	{ return static::code($light ? 105 : 45); }
	public static function bgCyan(		$light=false)	{ return static::code($light ? 106 : 46); }
	public static function bgGray(		$light=false)	{ return static::code($light ? 47 : 100); }
	public static function bgWhite(		$light=false)	{ return static::code($light ? 49 : 107); }

}
