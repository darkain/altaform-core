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

	public static function line() {
		return PHP_EOL;
	}

	public static function code($code, $text=NULL) {
		return ($text === NULL)
			? ("\033[" . $code . 'm')
			: ("\033[" . $code . 'm' . $text . static::reset());
	}

	public static function reset() {
		return static::code(0);
	}

	public static function bold($enabled=true, $text=NULL) {
		if (is_string($enabled)  &&  $text === NULL) { $text=$enabled; $enabled=true; }
		return static::code($enabled ? 1 : 21, $text);
	}

	public static function dim($enabled=true, $text=NULL) {
		if (is_string($enabled)  &&  $text === NULL) { $text=$enabled; $enabled=true; }
		return static::code($enabled ? 2 : 22, $text);
	}

	public static function underlined($enabled=true, $text=NULL) {
		if (is_string($enabled)  &&  $text === NULL) { $text=$enabled; $enabled=true; }
		return static::code($enabled ? 4 : 24, $text);
	}

	public static function blink($enabled=true, $text=NULL) {
		if (is_string($enabled)  &&  $text === NULL) { $text=$enabled; $enabled=true; }
		return static::code($enabled ? 5 : 25, $text);
	}

	public static function reverse($enabled=true, $text=NULL) {
		if (is_string($enabled)  &&  $text === NULL) { $text=$enabled; $enabled=true; }
		return static::code($enabled ? 7 : 27, $text);
	}

	public static function hidden($enabled=true, $text=NULL) {
		if (is_string($enabled)  &&  $text === NULL) { $text=$enabled; $enabled=true; }
		return static::code($enabled ? 8 : 28, $text);
	}




	////////////////////////////////////////////////////////////////////////////
	// FOREGROUND COLORS
	////////////////////////////////////////////////////////////////////////////
	public static function fgBlack($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 31 : 30, $text);
	}

	public static function fgRed($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 91 : 31, $text);
	}

	public static function fgGreen($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 92 : 32, $text);
	}

	public static function fgYellow($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 93 : 33, $text);
	}

	public static function fgBlue($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 94 : 34, $text);
	}

	public static function fgMagenta($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 95 : 35, $text);
	}

	public static function fgCyan($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 96 : 36, $text);
	}

	public static function fgGray($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 37 : 90, $text);
	}

	public static function fgWhite($light=false, $text=NULL) {
		if (is_string($light)  &&  $text === NULL) { $text=$light; $light=false; }
		return static::code($light ? 39 : 97, $text);
	}




	////////////////////////////////////////////////////////////////////////////
	// BACKGROUND COLORS
	////////////////////////////////////////////////////////////////////////////
	public static function bgBlack($light=false, $text=NULL) {
		return static::code($light ? 97 : 40, $text);
	}

	public static function bgRed($light=false, $text=NULL) {
		return static::code($light ? 101 : 41, $text);
	}

	public static function bgGreen($light=false, $text=NULL) {
		return static::code($light ? 102 : 42, $text);
	}

	public static function bgYellow($light=false, $text=NULL) {
		return static::code($light ? 103 : 43, $text);
	}

	public static function bgBlue($light=false, $text=NULL) {
		return static::code($light ? 104 : 44, $text);
	}

	public static function bgMagenta($light=false, $text=NULL) {
		return static::code($light ? 105 : 45, $text);
	}

	public static function bgCyan($light=false, $text=NULL) {
		return static::code($light ? 106 : 46, $text);
	}

	public static function bgGray($light=false, $text=NULL) {
		return static::code($light ? 47 : 100, $text);
	}

	public static function bgWhite($light=false, $text=NULL) {
		return static::code($light ? 49 : 107, $text);
	}


}
