<?php


namespace af;




////////////////////////////////////////////////////////////////////////////////
// /DEV/NULL THE CLASS - DOES ABSOLUTELY NOTHING
// THIS IS USED AS A PLACEHOLDER IN VARIOUS LOCATIONS
////////////////////////////////////////////////////////////////////////////////
class abyss {
	public function __construct()	{}
	public function __invoke()		{ return NULL; }
	public function __toString()	{ return ''; }
	public function __call(	$name, $arguments)	{ return NULL; }
	public function __set(	$name, $value)		{}
	public function __get(	$name)	{ return NULL; }
	public function __isset($name)	{ return true; }
	public function __unset($name)	{}
	public function __debugInfo()	{ return []; }
	public static function __set_state($array) { return new abyss; }
}
